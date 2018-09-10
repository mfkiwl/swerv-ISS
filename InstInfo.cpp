#include "InstInfo.hpp"

using namespace WdRiscv;

InstInfo::InstInfo(std::string name, InstId id,
		   uint32_t code, uint32_t mask,
		   InstType type,
		   OperandType op0Type, OperandMode op0Mode, uint32_t op0Mask,
		   OperandType op1Type, OperandMode op1Mode, uint32_t op1Mask,
		   OperandType op2Type, OperandMode op2Mode, uint32_t op2Mask)
  : name_(name), id_(id), code_(code), codeMask_(mask), type_(type),
    op0Mask_(op0Mask), op1Mask_(op1Mask), op2Mask_(op2Mask),
    op0Type_(op0Type), op1Type_(op1Type), op2Type_(op2Type),
    op0Mode_(op0Mode), op1Mode_(op1Mode), op2Mode_(op2Mode),
    opCount_(0)
{
  unsigned count = 0;

  if (op0Type != OperandType::None) count++;
  if (op1Type != OperandType::None) count++;
  if (op2Type != OperandType::None) count++;
  opCount_ = count;
}


InstInfoTable::InstInfoTable()
{
  setupInstVec();

  for (const auto& instInfo : instVec_)
    instMap_[instInfo.name()] = instInfo.instId();
}


const InstInfo&
InstInfoTable::getInstInfo(InstId id) const
{
  if (size_t(id) >= instVec_.size())
    return instVec_.front();
  return instVec_.at(size_t(id));
}


const InstInfo&
InstInfoTable::getInstInfo(const std::string& name) const
{
  const auto iter = instMap_.find(name);
  if (iter == instMap_.end())
    return instVec_.front();
  auto id = iter->second;
  return getInstInfo(id);
}


void
InstInfoTable::setupInstVec()
{
  uint32_t rdMask = 0x1f << 7;
  uint32_t rs1Mask = 0x1f << 15;
  uint32_t rs2Mask = 0x1f << 20;
  uint32_t immTop20 = 0xffff << 12;  // Immidiate: top 20 bits.
  uint32_t immTop12 = 0xfff << 20;   // Immidiate: top 12 bits.
  uint32_t immBeq = 0xfe000f80;
  uint32_t shamtMask = 0x01f00000;

  uint32_t low7Mask = 0x7f;                 // Opcode mask: lowest 7 bits
  uint32_t funct3Low7Mask = 0x707f;         // Funct3 and lowest 7 bits
  uint32_t top7Funct3Low7Mask = 0xfe00707f; // Top7, Funct3 and lowest 7 bits

  instVec_ =
    {
      // Base instructions
      { "illegal", InstId::illegal, 0xffffffff, 0xffffffff },

      { "lui", InstId::lui, 0x37, low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::Imm, OperandMode::None, immTop20 },

      { "auipc", InstId::auipc, 0x17, low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::Imm, OperandMode::None, immTop20 },

      { "jal", InstId::jal, 0x6f, low7Mask,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::Imm, OperandMode::None, immTop20 },

      { "jalr", InstId::jalr, 0x0067, funct3Low7Mask,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "beq", InstId::beq, 0x0063, funct3Low7Mask,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask,
	OperandType::Imm, OperandMode::None, immBeq },

      { "bne", InstId::bne, 0x1063, funct3Low7Mask,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask,
	OperandType::Imm, OperandMode::None, immBeq },

      { "blt", InstId::blt, 0x4063, funct3Low7Mask,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask,
	OperandType::Imm, OperandMode::None, immBeq },

      { "bge", InstId::bge, 0x5063, funct3Low7Mask,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask,
	OperandType::Imm, OperandMode::None, immBeq },

      { "bltu", InstId::bltu, 0x6063, funct3Low7Mask,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask,
	OperandType::Imm, OperandMode::None, immBeq },

      { "bgeu", InstId::bgeu, 0x7063, funct3Low7Mask,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask,
	OperandType::Imm, OperandMode::None, immBeq },

      { "lb", InstId::lb, 0x0003, funct3Low7Mask,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "lh", InstId::lh, 0x1003, funct3Low7Mask,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "lw", InstId::lw, 0x2003, funct3Low7Mask,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "lbu", InstId::lbu, 0x4003, funct3Low7Mask,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "lhu", InstId::lhu, 0x5003, funct3Low7Mask,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "sb", InstId::sb, 0x0023, funct3Low7Mask,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask,
	OperandType::Imm, OperandMode::None, immBeq },

      { "sh", InstId::sh, 0x1023, funct3Low7Mask,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask,
	OperandType::Imm, OperandMode::None, immBeq },

      { "sw", InstId::sw, 0x2023, funct3Low7Mask,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask,
	OperandType::Imm, OperandMode::None, immBeq },

      { "addi", InstId::addi, 0x0013, funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "slti", InstId::slti, 0x2013, funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "sltiu", InstId::sltiu, 0x3013, funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "xori", InstId::xori, 0x4013, funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "ori", InstId::ori, 0x6013, funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "andi", InstId::andi, 0x7013, funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "slli", InstId::slli, 0x1013, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, shamtMask },

      { "srli", InstId::srli, 0x5013, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, shamtMask },

      { "srai", InstId::srai, 0x40005013, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, shamtMask },

      { "add", InstId::add, 0x0033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "sub", InstId::sub, 0x40000033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "sll", InstId::sll, 0x1033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "slt", InstId::slt, 0x2033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "sltu", InstId::sltu, 0x3033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "xor", InstId::xor_, 0x4033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "srl", InstId::srl, 0x5033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "sra", InstId::sra, 0x40001033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "or", InstId::or_, 0x6033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "and", InstId::and_, 0x7033, top7Funct3Low7Mask,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "fence", InstId::fence, 0x000f, 0xf00fffff,
	InstType::Int,
	OperandType::Imm, OperandMode::None, 0x0f000000,   // TBD: check
	OperandType::Imm, OperandMode::None, 0x00f00000 }, // TBD: check

      { "fencei", InstId::fencei, 0x100f, 0xffffffff },

      { "ecall", InstId::ecall, 0x00000073, 0xffffffff },
      { "ebreak", InstId::ebreak, 0x00100073, 0xffffffff },

      // CSR
      { "csrrw", InstId::csrrw, 0x1073, funct3Low7Mask,
	InstType::Csr,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::CsReg, OperandMode::ReadWrite, immTop12 },

      { "csrrs", InstId::csrrs, 0x2073, funct3Low7Mask,
	InstType::Csr,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::CsReg, OperandMode::ReadWrite, immTop12 },

      { "csrrc", InstId::csrrc, 0x3073, funct3Low7Mask,
	InstType::Csr,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::CsReg, OperandMode::ReadWrite, immTop12 },


      { "csrrwi", InstId::csrrwi,  0x5073, funct3Low7Mask,
	InstType::Csr,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::Imm, OperandMode::None, rs1Mask,
	OperandType::CsReg, OperandMode::ReadWrite, immTop12 },

      { "csrrsi", InstId::csrrsi, 0x6073, funct3Low7Mask,
	InstType::Csr,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::Imm, OperandMode::None, rs1Mask,
	OperandType::CsReg, OperandMode::ReadWrite, immTop12 },

      { "csrrci", InstId::csrrci, 0x7073, funct3Low7Mask,
	InstType::Csr,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::Imm, OperandMode::None, rs1Mask,
	OperandType::CsReg, OperandMode::ReadWrite, immTop12 },

      // rv64i: TBD: fix
      { "lwu", InstId::lwu, 0, 0 },
      { "ld", InstId::ld, 0, 0 },
      { "sd", InstId::sd, 0, 0 },
      { "addiw", InstId::addiw, 0, 0 },
      { "slliw", InstId::slliw, 0, 0 },
      { "srliw", InstId::srliw, 0, 0 },
      { "sraiw", InstId::sraiw, 0, 0 },
      { "addw", InstId::addw, 0, 0 },
      { "subw", InstId::subw, 0, 0 },
      { "sllw", InstId::sllw, 0, 0 },
      { "srlw", InstId::srlw, 0, 0 },
      { "sraw", InstId::sraw, 0, 0 },

      // Mul/div
      { "mul", InstId::mul, 0x02000033, top7Funct3Low7Mask,
	InstType::Multiply,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "mulh", InstId::mulh, 0x02001033, top7Funct3Low7Mask,
	InstType::Multiply,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "mulhsu", InstId::mulhsu, 0x02002033, top7Funct3Low7Mask,
	InstType::Multiply,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "mulhu", InstId::mulhu, 0x02003033, top7Funct3Low7Mask,
	InstType::Multiply,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "div", InstId::div, 0x02004033, top7Funct3Low7Mask,
	InstType::Divide,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "divu", InstId::divu, 0x02005033, top7Funct3Low7Mask,
	InstType::Divide,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "rem", InstId::rem, 0x02006033, top7Funct3Low7Mask,
	InstType::Divide,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      { "remu", InstId::remu, 0x02007033, top7Funct3Low7Mask,
	InstType::Divide,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::IntReg, OperandMode::Read, rs2Mask },

      // 64-bit mul/div: TBD fix
      { "mulw", InstId::mulw, 0, 0 },
      { "divw", InstId::divw, 0, 0 },
      { "divuw", InstId::divuw, 0, 0 },
      { "remw", InstId::remw, 0, 0 },
      { "remuw", InstId::remuw, 0, 0 },

      // Atomic: TBD fix
      { "lr_w", InstId::lr_w, 0, 0 },
      { "sc_w", InstId::sc_w, 0, 0 },
      { "amoswap_w", InstId::amoswap_w, 0, 0 },
      { "amoadd_w", InstId::amoadd_w, 0, 0 },
      { "amoxor_w", InstId::amoxor_w, 0, 0 },
      { "amoand_w", InstId::amoand_w, 0, 0 },
      { "amoor_w", InstId::amoor_w, 0, 0 },
      { "amomin_w", InstId::amomin_w, 0, 0 },
      { "amomax_w", InstId::amomax_w, 0, 0 },
      { "amominu_w", InstId::amominu_w, 0, 0 },
      { "amomaxu_w", InstId::amomaxu_w, 0, 0 },

      // 64-bit atomic: TBD fix
      { "lr_d", InstId::lr_d, 0, 0 },
      { "sc_d", InstId::sc_d, 0, 0 },
      { "amoswap_d", InstId::amoswap_d, 0, 0 },
      { "amoadd_d", InstId::amoadd_d, 0, 0 },
      { "amoxor_d", InstId::amoxor_d, 0, 0 },
      { "amoand_d", InstId::amoand_d, 0, 0 },
      { "amoor_d", InstId::amoor_d, 0, 0 },
      { "amomin_d", InstId::amomin_d, 0, 0 },
      { "amomax_d", InstId::amomax_d, 0, 0 },
      { "amominu_d", InstId::amominu_d, 0, 0 },
      { "amomaxu_d", InstId::amomaxu_d, 0, 0 },

      // rv32f: TBD fix
      { "flw", InstId::flw, 0, 0 },
      { "fsw", InstId::fsw, 0, 0 },
      { "fmadd_s", InstId::fmadd_s, 0, 0 },
      { "fmsub_s", InstId::fmsub_s, 0, 0 },
      { "fnmsub_s", InstId::fnmsub_s, 0, 0 },
      { "fnmadd_s", InstId::fnmadd_s, 0, 0 },
      { "fadd_s", InstId::fadd_s, 0, 0 },
      { "fsub_s", InstId::fsub_s, 0, 0 },
      { "fmul_s", InstId::fmul_s, 0, 0 },
      { "fdiv_s", InstId::fdiv_s, 0, 0 },
      { "fsqrt_s", InstId::fsqrt_s, 0, 0 },
      { "fsgnj_s", InstId::fsgnj_s, 0, 0 },
      { "fsgnjn_s", InstId::fsgnjn_s, 0, 0 },
      { "fsgnjx_s", InstId::fsgnjx_s, 0, 0 },
      { "fmin_s", InstId::fmin_s, 0, 0 },
      { "fmax_s", InstId::fmax_s, 0, 0 },
      { "fcvt_w_s", InstId::fcvt_w_s, 0, 0 },
      { "fcvt_wu_s", InstId::fcvt_wu_s, 0, 0 },
      { "fmv_x_w", InstId::fmv_x_w, 0, 0 },
      { "feq_s", InstId::feq_s, 0, 0 },
      { "flt_s", InstId::flt_s, 0, 0 },
      { "fle_s", InstId::fle_s, 0, 0 },
      { "fclass_s", InstId::fclass_s, 0, 0 },
      { "fcvt_s_w", InstId::fcvt_s_w, 0, 0 },
      { "fcvt_s_wu", InstId::fcvt_s_wu, 0, 0 },
      { "fmv_w_x", InstId::fmv_w_x, 0, 0 },

      // rv64f: TBD fix
      { "fcvt_l_s", InstId::fcvt_l_s, 0, 0 },
      { "fcvt_lu_s", InstId::fcvt_lu_s, 0, 0 },
      { "fcvt_s_l", InstId::fcvt_s_l, 0, 0 },
      { "fcvt_s_lu", InstId::fcvt_s_lu, 0, 0 },

      // rv32d: TBD fix
      { "fld", InstId::fld, 0, 0 },
      { "fsd", InstId::fsd, 0, 0 },
      { "fmadd_d", InstId::fmadd_d, 0, 0 },
      { "fmsub_d", InstId::fmsub_d, 0, 0 },
      { "fnmsub_d", InstId::fnmsub_d, 0, 0 },
      { "fnmadd_d", InstId::fnmadd_d, 0, 0 },
      { "fadd_d", InstId::fadd_d, 0, 0 },
      { "fsub_d", InstId::fsub_d, 0, 0 },
      { "fmul_d", InstId::fmul_d, 0, 0 },
      { "fdiv_d", InstId::fdiv_d, 0, 0 },
      { "fsqrt_d", InstId::fsqrt_d, 0, 0 },
      { "fsgnj_d", InstId::fsgnj_d, 0, 0 },
      { "fsgnjn_d", InstId::fsgnjn_d, 0, 0 },
      { "fsgnjx_d", InstId::fsgnjx_d, 0, 0 },
      { "fmin_d", InstId::fmin_d, 0, 0 },
      { "fmax_d", InstId::fmax_d, 0, 0 },
      { "fcvt_s_d", InstId::fcvt_s_d, 0, 0 },
      { "fcvt_d_s", InstId::fcvt_d_s, 0, 0 },
      { "feq_d", InstId::feq_d, 0, 0 },
      { "flt_d", InstId::flt_d, 0, 0 },
      { "fle_d", InstId::fle_d, 0, 0 },
      { "fclass_d", InstId::fclass_d, 0, 0 },
      { "fcvt_w_d", InstId::fcvt_w_d, 0, 0 },
      { "fcvt_wu_d", InstId::fcvt_wu_d, 0, 0 },
      { "fcvt_d_w", InstId::fcvt_d_w, 0, 0 },
      { "fcvt_d_wu", InstId::fcvt_d_wu, 0, 0 },

      // rv64f: TBD fix
      { "fcvt_l_d", InstId::fcvt_l_d, 0, 0 },
      { "fcvt_lu_d", InstId::fcvt_lu_d, 0, 0 },
      { "fmv_x_d", InstId::fmv_x_d, 0, 0 },
      { "fcvt_d_l", InstId::fcvt_d_l, 0, 0 },
      { "fcvt_d_lu", InstId::fcvt_d_lu, 0, 0 },
      { "fmv_d_x", InstId::fmv_d_x, 0, 0 },

      // Privileged
      { "mret", InstId::mret, 0, 0 },
      { "uret", InstId::uret, 0, 0 },
      { "sret", InstId::sret, 0, 0 },
      { "wfi", InstId::wfi, 0, 0 },

      // Compressed insts. The operand bits are "swizzled"
      // the masks are not use in obtaining operands. We
      // set the masks to zero.
      { "c.addi4spn", InstId::c_addi4spn, 0x0000, 0xe003,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.fld", InstId::c_fld, 0x2000, 0xe003,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.lq", InstId::c_lq, 0x2000, 0xe003, 
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.lw", InstId::c_lw, 04000, 0xe003,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.flw", InstId::c_flw, 0x6000, 0xe003,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.ld", InstId::c_ld, 0x6000, 0xe003,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.fsd", InstId::c_fsd, 0xa000, 0xe003,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.sq", InstId::c_sq, 0xa000, 0xe003,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.sw", InstId::c_sw, 0xc000, 0xe003,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.fsw", InstId::c_fsw, 0xe000, 0xe003,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.sd", InstId::c_sd, 0xe000, 0xe003,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.addi", InstId::c_addi, 0x0001, 0xe003,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.jal", InstId::c_jal, 0x0001, 0xe003,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.li", InstId::c_li, 0x4001, 0xe003,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.addi16sp", InstId::c_addi16sp, 0x6006, 0xef83,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.lui", InstId::c_lui, 0x6001, 0xe003,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.srli", InstId::c_srli, 0x8001, 0xec03,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, shamtMask },

      { "c.srli64", InstId::c_srli64, 0x8001, 0xfc83,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.srai", InstId::c_srai, 0x8401, 0xec03,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.srai64", InstId::c_srai64, 0x8401, 0xfc83,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.andi", InstId::c_andi, 0x8801, 0xec03,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.sub", InstId::c_sub, 0x8c01, 0xfc63,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0 },

      { "c.xor", InstId::c_xor, 0x8c21, 0xfc63,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0 },

      { "c.or", InstId::c_or, 0x8c41, 0xfc63,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0 },

      { "c.and", InstId::c_and, 0x8c61, 0xfc63,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0 },

      { "c.subw", InstId::c_subw, 0x9c01, 0xfc63,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0 },

      { "c.addw", InstId::c_addw, 0x9c21, 0xfc63,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0 },

      { "c.j", InstId::c_j, 0xa001, 0xe003,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.beqz", InstId::c_beqz, 0xc001, 0xe003,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.bnez", InstId::c_bnez, 0xe001, 0xe003,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.slli", InstId::c_slli, 0x0002, 0xe003,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, shamtMask },

      { "c.slli64", InstId::c_slli64, 0x0002, 0xf083,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, shamtMask },

      { "c.fldsp", InstId::c_fldsp, 0x2002, 0xe003,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, shamtMask },

      { "c.lwsp", InstId::c_lwsp, 0x4002, 0xe003,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.flwsp", InstId::c_flwsp, 0x6002, 0xe003,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.fldsp", InstId::c_flwsp, 0x6002, 0xe003,
	InstType::Load,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.jr", InstId::c_jr, 0x8002, 0xf07f,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "c.mv", InstId::c_mv, 0x8002, 0xf003,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, rdMask,
	OperandType::IntReg, OperandMode::Read, rs1Mask,
	OperandType::Imm, OperandMode::None, immTop12 },

      { "c.ebreak", InstId::c_ebreak, 0x9002, 0xffff },

      { "c.jalr", InstId::c_jalr, 0x9002, 0xf07f,
	InstType::Branch,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.add", InstId::c_add, 0x9002, 0xf003,
	InstType::Int,
	OperandType::IntReg, OperandMode::Write, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0 },

      { "c.fsdsp", InstId::c_fsdsp, 0xa002, 0xe003,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.swsp", InstId::c_swsp, 0xc002, 0xe003,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },

      { "c.fswsp", InstId::c_fswsp, 0xe002, 0xe003,
	InstType::Store,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::IntReg, OperandMode::Read, 0,
	OperandType::Imm, OperandMode::None, 0 },
    };
}


