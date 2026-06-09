#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parserIR.h"
#include "symtable.h"
#include "utils.h"

#define MAX_TOKENS 12

static void set_field(Parser_Instruction *inst, int i, Parser_Field f) {
    //sets a specific field in the instruction
    if (i == 0) inst->f0 = f;
    else if (i == 1) inst->f1 = f;
    else if (i == 2) inst->f2 = f;
    else if (i == 3) inst->f3 = f;
    else {
        fprintf(stderr, "Too many fields\n");
        exit(EXIT_FAILURE);
    }
}

static void strip_comments(char *line) {
    char *comment = strstr(line, "//");
    if (comment) *comment = '\0';
}

//remove whitespaces (leadig and trailing)
static void trim(char *s) {
    char *start = s;
    while (isspace((unsigned char)*start)) start++;

    if (start != s) memmove(s, start, strlen(start) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        len--;
    }
}

//split the string (assembly instruction) into words
//returns the number of words in the instruction
int tokenize(char *line, char *tokens[]) {
    int n = 0;
    char *p = line;

    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == ',' || *p == '\n') p++;
        if (*p == '\0') break;

        tokens[n++] = p;

        if (*p == '[') {
            while (*p && *p != ']') p++;
            if (*p == ']') p++;
            if (*p == '!') p++;
        } else {
            while (*p && *p != ' ' && *p != '\t' && *p != ',' && *p != '\n') p++;
        }

        if (*p) {
            *p = '\0';
            p++;
        }
    }

    return n;
}

//returns the mnemonic associated with the instruction
static Mnemonic parse_mnemonic(const char *s) {
    if (!strcmp(s, ".int")) return DIRECTIVE;

    if (!strcmp(s, "add")) return ADD;
    if (!strcmp(s, "adds")) return ADDS;
    if (!strcmp(s, "sub")) return SUB;
    if (!strcmp(s, "subs")) return SUBS;
    if (!strcmp(s, "cmp")) return CMP;
    if (!strcmp(s, "cmn")) return CMN;
    if (!strcmp(s, "neg")) return NEG;
    if (!strcmp(s, "negs")) return NEGS;

    if (!strcmp(s, "and")) return AND;
    if (!strcmp(s, "ands")) return ANDS;
    if (!strcmp(s, "bic")) return BIC;
    if (!strcmp(s, "bics")) return BICS;
    if (!strcmp(s, "eor")) return EOR;
    if (!strcmp(s, "orr")) return ORR;
    if (!strcmp(s, "eon")) return EON;
    if (!strcmp(s, "orn")) return ORN;
    if (!strcmp(s, "tst")) return TST;

    if (!strcmp(s, "movk")) return MOVK;
    if (!strcmp(s, "movn")) return MOVN;
    if (!strcmp(s, "movz")) return MOVZ;
    if (!strcmp(s, "mov")) return MOV;
    if (!strcmp(s, "mvn")) return MVN;

    if (!strcmp(s, "madd")) return MADD;
    if (!strcmp(s, "msub")) return MSUB;
    if (!strcmp(s, "mul")) return MUL;
    if (!strcmp(s, "mneg")) return MNEG;

    if (!strcmp(s, "b")) return B;
    if (!strcmp(s, "br")) return BR;

    if (!strcmp(s, "ldr")) return LDR;
    if (!strcmp(s, "str")) return STR;

    if (!strncmp(s, "b.", 2)) return BCOND;

    fprintf(stderr, "Unknown mnemonic: %s\n", s);
    exit(EXIT_FAILURE);
}

//returns the condition
static conds parse_cond(const char *s) {
    if (!strcmp(s, "eq")) return EQ;
    if (!strcmp(s, "ne")) return NE;
    if (!strcmp(s, "ge")) return GE;
    if (!strcmp(s, "lt")) return LT;
    if (!strcmp(s, "gt")) return GT;
    if (!strcmp(s, "le")) return LE;
    if (!strcmp(s, "al")) return AL;

    fprintf(stderr, "Invalid condition: %s\n", s);
    exit(EXIT_FAILURE);
}

//returns the register in the instruction
static Parser_Register parse_register(const char *s) {
    Parser_Register r;

    if (!strcmp(s, "xzr")) {
        r.sf = true;
        r.index = 31;
        return r;
    }

    if (!strcmp(s, "wzr")) {
        r.sf = false;
        r.index = 31;
        return r;
    }

    if (!strcmp(s, "sp")) {
        r.sf = true;
        r.index = 31;
        return r;
    }

    if (!strcmp(s, "wsp")) {
        r.sf = false;
        r.index = 31;
        return r;
    }

    if (s[0] == 'x') {
        r.sf = true;
        r.index = (uint8_t)atoi(s + 1);
        return r;
    }

    if (s[0] == 'w') {
        r.sf = false;
        r.index = (uint8_t)atoi(s + 1);
        return r;
    }

    fprintf(stderr, "Invalid register: %s\n", s);
    exit(EXIT_FAILURE);
}

//returns immediate value
static int32_t parse_immediate(const char *s) {
    if (s[0] == '#') s++;
    return (int32_t)strtol(s, NULL, 0);
}

//returns the shift
static Shift_Kind parse_shift_kind(const char *s) {
    if (!strcmp(s, "lsl")) return LSL;
    if (!strcmp(s, "lsr")) return LSR;
    if (!strcmp(s, "asr")) return ASR;
    if (!strcmp(s, "ror")) return ROR;

    fprintf(stderr, "Invalid shift: %s\n", s);
    exit(EXIT_FAILURE);
}

//The next 5 functions create fields for their respective field types

static Parser_Field make_reg_field(Parser_Register r) {
    Parser_Field f;
    f.field = REGISTER;
    f.field_data.reg = r;
    return f;
}

static Parser_Field make_imm_field(int32_t imm) {
    Parser_Field f;
    f.field = IMMEDIATE;
    f.field_data.immediate = imm;
    return f;
}

static Parser_Field make_shift_field(Shift_Kind kind, uint32_t amount) {
    Parser_Field f;
    f.field = SHIFT;
    f.field_data.shift.kind = kind;
    f.field_data.shift.shift_amount = amount;
    return f;
}

static Parser_Field make_literal_label_field(char *label) {
    Parser_Field f;
    f.field = ADDRESS;
    f.field_data.address.kind = LITERAL;
    f.field_data.address.address_data.literal.literal_kind = LIT_LABEL;
    f.field_data.address.address_data.literal.data.label = strdup(label);
    return f;
}

static Parser_Field make_literal_addr_field(uint64_t addr) {
    Parser_Field f;
    f.field = ADDRESS;
    f.field_data.address.kind = LITERAL;
    f.field_data.address.address_data.literal.literal_kind = LIT_ADDR;
    f.field_data.address.address_data.literal.data.int_address = addr;
    return f;
}

static Parser_Field parse_bracket_address(char *token) {
    Parser_Field f;
    f.field = ADDRESS;

    bool pre_indexed = false;
    size_t len = strlen(token);

    if (len > 0 && token[len - 1] == '!') {
        pre_indexed = true;
        token[len - 1] = '\0';
    }

    if (token[0] != '[') {
        fprintf(stderr, "Invalid address: %s\n", token);
        exit(EXIT_FAILURE);
    }

    token++;
    char *end = strchr(token, ']');
    if (end) *end = '\0';

    char *comma = strchr(token, ',');

    if (!comma) {
        f.field_data.address.kind = UNSIGNED_OFFSET;
        f.field_data.address.address_data.unsigned_offset.xn = parse_register(token);
        f.field_data.address.address_data.unsigned_offset.imm = 0;
        return f;
    }

    *comma = '\0';
    char *base = token;
    char *second = comma + 1;

    while (isspace((unsigned char)*second)) second++;

    if (second[0] == '#') {
        int32_t imm = parse_immediate(second);

        if (pre_indexed) {
            f.field_data.address.kind = PRE_INDEXED;
            f.field_data.address.address_data.pre_post_index.xn = parse_register(base);
            f.field_data.address.address_data.pre_post_index.simm = imm;
        } else {
            f.field_data.address.kind = UNSIGNED_OFFSET;
            f.field_data.address.address_data.unsigned_offset.xn = parse_register(base);
            f.field_data.address.address_data.unsigned_offset.imm = (uint32_t)imm;
        }

        return f;
    }

    f.field_data.address.kind = REGISTER_OFFSET;
    f.field_data.address.address_data.register_offset.xn = parse_register(base);
    f.field_data.address.address_data.register_offset.xm = parse_register(second);
    return f;
}

static Parser_Field parse_normal_field(char *token) {
    if (token[0] == 'x' || token[0] == 'w') {
        return make_reg_field(parse_register(token));
    }

    if (token[0] == '#') {
        return make_imm_field(parse_immediate(token));
    }

    if (token[0] == '[') {
        return parse_bracket_address(token);
    }

    return make_literal_label_field(token);
}

int parser(
    char *assembly_instruction,
    Sym_Table *symbol_table,
    Parser_Instruction *parsed,
    uint32_t current_address
) {
    strip_comments(assembly_instruction);
    trim(assembly_instruction);

    parsed->address = current_address;
    parsed->num_fields = 0;
    parsed->cond = AL;

    if (assembly_instruction[0] == '\0') {
        return 0;
    }

    char *colon = strchr(assembly_instruction, ':');
    if (colon != NULL) {
        *colon = '\0';
        trim(assembly_instruction);
        if (!insert_address(symbol_table, assembly_instruction, current_address)) {
            fprintf(stderr, "Failed to insert label: %s\n", assembly_instruction);
            exit(EXIT_FAILURE);
        }

        parsed->mnemonic = LABEL;
        parsed->num_fields = 0;
        return 0;
    }

    char *tokens[MAX_TOKENS];
    int n = tokenize(assembly_instruction, tokens);

    if (n == 0) return 0;

    parsed->mnemonic = parse_mnemonic(tokens[0]);

    if (parsed->mnemonic == BCOND) {
        parsed->cond = parse_cond(tokens[0] + 2);
    }

    if (parsed->mnemonic == DIRECTIVE) {
        parsed->f0 = make_imm_field((int32_t)strtol(tokens[1], NULL, 0));
        parsed->num_fields = 1;
        return 0;
    }

    for (int i = 1; i < n; i++) {
        Parser_Field f;

        if (!strcmp(tokens[i], "lsl") ||
            !strcmp(tokens[i], "lsr") ||
            !strcmp(tokens[i], "asr") ||
            !strcmp(tokens[i], "ror")) {

            if (i + 1 >= n) {
                fprintf(stderr, "Shift missing amount\n");
                exit(EXIT_FAILURE);
            }

            f = make_shift_field(
                parse_shift_kind(tokens[i]),
                (uint32_t)parse_immediate(tokens[i + 1])
            );

            i++;
        } else {
            if (parsed->mnemonic == B || parsed->mnemonic == BCOND) {
                if (tokens[1][0] == '#') {
                    f = make_literal_addr_field((uint64_t)parse_immediate(tokens[1]));
                } else {
                    f = make_literal_label_field(tokens[1]);
                }
            } else {
                f = parse_normal_field(tokens[i]);
            }
        }

        set_field(parsed, parsed->num_fields, f);
        parsed->num_fields++;
    }

    if ((parsed->mnemonic == LDR || parsed->mnemonic == STR) &&
        parsed->num_fields == 3 &&
        parsed->f1.field == ADDRESS &&
        parsed->f1.field_data.address.kind == UNSIGNED_OFFSET &&
        parsed->f2.field == IMMEDIATE) {

        Parser_Register xn =
            parsed->f1.field_data.address.address_data.unsigned_offset.xn;

        int32_t simm = parsed->f2.field_data.immediate;

        parsed->f1.field_data.address.kind = POST_INDEXED;
        parsed->f1.field_data.address.address_data.pre_post_index.xn = xn;
        parsed->f1.field_data.address.address_data.pre_post_index.simm = simm;

        parsed->num_fields = 2;
    }

    return 1;
}