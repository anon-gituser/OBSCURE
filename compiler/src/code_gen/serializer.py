from typing import Final
from math import ceil, log2
from IR import Opcode, ImmOperand, RegOperand, LLI, LLIRProgram
from schwaemm import schwaemm128128_encrypt
from pysodium import crypto_box_seal
from .keys import shared_key, pubkey

# RegOperand or ImmOperand or None
NUL_FLAG = '00'
REG_FLAG = '01'
IMM_FLAG = '10'


class ID:
    def __init__(self, instrID: int, outputID: int):
        self.instrID  : Final = instrID
        self.outputID : Final = outputID

    def __str__(self):
        return f"(InstrID, OutputID) = ({self.instrID}, {self.outputID})"


# def create_bits_dict(count: int, bitlen: int):
#     bits_dict = {}
#     for i in range(count):
#         bits_dict[i] = BitArray(f'uint:{bitlen}={i}')

#     return bits_dict

def create_opdict(count: int):
    """count = 15, each opcode is represented by 4 bits"""
    opdict = {}
    for i in range(count):
        b = bin(i)[2:]
        opdict[i] = '0'*(4-len(b)) + b
    return opdict

def create_memdict(count: int, bytelen: int):
    memdict = {}
    for i in range(count):
        memdict[i] = (i).to_bytes(bytelen, byteorder='big')
    return memdict

# def encode_num2bits(number: int, bitlen: int):
#     return BitArray(f'uint:{bitlen}={number}')

def uint_tobytes(n: int, bytelen: int):
    return (n).to_bytes(bytelen, byteorder='big')

def bin_tobytes(bitstr: str):
    bitlen = len(bitstr)
    assert bitlen % 8 == 0
    n = int(bitstr, 2)
    return (n).to_bytes(bitlen//8, byteorder='big')

def serialize_metadata(config):
    meta_bytecode = bytes()

    # version (32)
    meta_bytecode += uint_tobytes(config.version, 4)
    # word_size (32)
    meta_bytecode += uint_tobytes(config.word_size, 4)
    # LLMI_max_input_count (l_in) (32)
    meta_bytecode += uint_tobytes(config.l_in, 4)
    # LLMI_max_output_count (l_out) (32)
    meta_bytecode += uint_tobytes(config.l_out, 4)
    # register_count (r) (32)
    meta_bytecode += uint_tobytes(config.r, 4)
    # LLS_max_length (s) (32)
    meta_bytecode += uint_tobytes(config.s, 4)

    return meta_bytecode

def serialize_lls(lls, register: dict, opdict: dict, config):
    lls_bytecode = bytes()

    for lli in lls:
        if lli.is_nop():
            lli_code = opdict[Opcode.NOP.value] + NUL_FLAG + NUL_FLAG
            lls_bytecode += lli_code
            continue

        # opcode (4)
        op_bin = opdict[lli.opcode.value]

        # dst (32)
        dst_code = register[lli.dst.r]

        # src1 (32)
        src1_code = bytes()
        if isinstance(lli.src1, RegOperand):
            op_bin += REG_FLAG
            src1_code += register[lli.src1.r]
        elif isinstance(lli.src1, ImmOperand):
            op_bin += IMM_FLAG
            src1_code += uint_tobytes(lli.src1.imm, config.word_size // 8)
        else:
            raise ValueError('Invalid Operand.')

        # src2
        src2_code = bytes()
        if lli.src2 is None:
            op_bin += NUL_FLAG
            # src2_code is skipped
        elif isinstance(lli.src2, RegOperand):
            op_bin += REG_FLAG
            src2_code += register[lli.src2.r]
        elif isinstance(lli.src2, ImmOperand):
            op_bin += IMM_FLAG
            src2_code += uint_tobytes(lli.src2.imm, config.word_size // 8)
        else:
            raise ValueError('Invalid Operand.')

        op_code = bin_tobytes(op_bin)
        lli_code = op_code + dst_code + src1_code + src2_code
        lls_bytecode += lli_code

    return lls_bytecode

def serialize(ir:LLIRProgram, config):
    outputs = set(ir.outputs)
    l_out = config.l_out

    bytecode = bytes()
    # bytelen
    lb_o = ((ceil(log2(l_out)) + 7) & (-8)) // 8             # for 1 outputID
    lb_m = ((ceil(log2(ir.memory_count)) + 7) & (-8)) // 8   # for 1 memory cell
    lb_r = ((ceil(log2(config.r)) + 7) & (-8)) // 8          # for 1 register

    # metadata (6*32)
    bytecode += serialize_metadata(config)

    # memory_count (32)
    bytecode += uint_tobytes(ir.memory_count, 4)
    memory = create_memdict(ir.memory_count, lb_m)

    # program header (512)
    ct_sk = crypto_box_seal(shared_key, pubkey)
    bytecode += ct_sk

    # input_count (lb_m)
    bytecode += uint_tobytes(len(ir.inputs), lb_m)

    # inputs (lb_m * input_count)
    id_dict = dict() # dict of (InstrID, OutputID)
    for (idx, inp) in enumerate(ir.inputs):
        bytecode += memory[inp.m]
        instrID  = idx//l_out + 1
        outputID = idx % l_out
        id_dict[inp.m] = ID(instrID, outputID)

    # output_count (lb_m)
    bytecode += uint_tobytes(len(ir.outputs), lb_m)

    # outputs (lb_m * output_count)
    for out in ir.outputs:
        bytecode += memory[out.m]

    # LLMI_count (32)
    bytecode += uint_tobytes(len(ir.instrs), 4)

    # Flush header to output
    config.outfile.write(bytecode)

    # LLMI
    opcode = create_opdict(len(Opcode))
    register = create_memdict(config.r, lb_r)
    for (i, llmi) in enumerate(ir.instrs):
        instrID = (i+1) + (len(ir.inputs)//l_out + 1)

        # input_count (lb_m)
        input_count_bstr = uint_tobytes(len(llmi.inputs), lb_m)

        # inputs (lb_m * input_count)
        inputIDs     = [] # list of (instrID, outputID)
        inputs_bstr  = bytes()
        for inp in llmi.inputs:
            inputs_bstr += memory[inp.m]
            inputIDs.append(id_dict[inp.m])

        # output_count (lb_m)
        output_count_bstr = uint_tobytes(len(llmi.outputs), lb_m)

        # outputs (lb_m * output_count)
        outputs_bstr = bytes()
        for (outputID, out) in enumerate(llmi.outputs):
            outputs_bstr  += memory[out.m]
            id_dict[out.m] = ID(instrID, outputID)

        # InstrID (32)
        instrID_bstr = uint_tobytes(instrID, 4)

        # RevealFlag (8)
        reveal_flag = 1 if [ True for out in llmi.outputs
                             if out in outputs ] != [] else 0
        rflag_bitstr = bin(reveal_flag)[2:]
        rflag_bitstr = (8-len(rflag_bitstr))*'0' + rflag_bitstr
        rflag_bstr = bin_tobytes(rflag_bitstr)

        # inputIDs ((lb_o + 32)*input_count)
        inputIDs_bstr = bytes()
        for id in inputIDs:
            inputIDs_bstr += uint_tobytes(id.instrID, 4)
            inputIDs_bstr += uint_tobytes(id.outputID, lb_o)

        # LLS
        lls_code_bstr = serialize_lls(llmi.seq.instrs, register, opcode, config)
        msg   = lls_code_bstr
        ad    = (instrID_bstr + rflag_bstr + \
                    input_count_bstr + inputIDs_bstr + output_count_bstr)
        nonce = uint_tobytes(instrID, 32)
        lls_code_bstr  = schwaemm128128_encrypt(msg, ad, nonce, shared_key)

        # LLS_bytelen (32)
        lls_bytelen_bstr = uint_tobytes(len(lls_code_bstr), 4)

        llmi_code = input_count_bstr  + inputs_bstr       \
                  + output_count_bstr + outputs_bstr      \
                  + instrID_bstr                          \
                  + rflag_bstr                            \
                  + inputIDs_bstr                         \
                  + lls_bytelen_bstr  + lls_code_bstr

        config.outfile.write(llmi_code)
