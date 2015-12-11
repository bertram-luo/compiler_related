#include <stdlib.h>
#include <stdio.h>
#include <stack>
#include <vector>
using std::vector;
using std::stack;
//TOOD type_name := spec_qualifier_list {abstract declarator} // type_name = 'int'
//DONE  exp := assignment_exp {, assignment_exp}
//TODO  assignment_exp := condition_exp | unary_exp assignment_operator assignment_exp
//DONE  assignment_operator := '=' | '*=' | '/=' | '%=' | '+=' | '-=' || '<<=' || '>>=' || '&=' || '^=' | '|='
//DONE  conditional_exp := logical_or_exp | logical_or_exp '?' exp ':' conditional_exp
//TODO  const_exp := conditional_exp
//DONE  logical_or_exp := logical_and_exp {{ '||' logical_and_exp}}
//DONE  logical_and_exp := inclusive_or_ex {'&&' inclusive_or_exp}
//DONE  inclusive_or_exp := exclusive_or_exp {'|' exclusive_or_exp}
//DONE  exclusive_or_exp := and_exp {'^' and_exp}
//DONE  and_exp := equality_exp {'&' equality_exp}
//DONE  equality_exp := relational_exp {('!='|'==') relational_exp}
//DONE  relational_exp := shift_expression {('<' | '>' | '<=' | '>=') shift_expression}
//DONE  shift_expression := additive_exp {('<<'| '>>') additive_exp}
//DONE  additive_expression := mult_exp {('+'|'-') mult_exp}
//DONE  mult_exp := cast_exp {('*' | '/' | '%') cast_exp}
//DONE  cast_exp : unary_exp | '(' type_name ')' cast_exp
//DONE unary_exp := postfix_exp | '++' unary_exp | '--' unary_exp | unary_operator cast_exp |TODO 'sizeof' unary_exp |TODO 'sizeof' '(' type_name ')'
//DONE unary_operator := TODO '&' | TODO '*' | '+' | '-' | '~' | '!'
//TODO  postfix_exp := primary_exp {'[' exp ']' | '(' ')' | '(' argument_exp_list ')' | '.' id | '->' id | '++' | '--'}
//DONE  primary_expression := id | const | string | '(' exp ')'
//DONE argument_exp_list := assignment_exp {{, assignment_exp}}
//DONE const := int_const | char_const |TODO float_const | TODO enumeration_const
//
//
enum TOK_TYPE {END, 
    INT_CONST, CHAR_CONST, STRING, ID, L_PAR, R_PAR, L_BR, R_BR, L_CBR, R_CBR,
    SINC, SDEC, FUNC, IDX,
    TINT, POS, NEG, PINC, PDEC, LNOT, NOT,
    MUL, DIV, MOD,
    ADD, SUB,
    EXP, SHL, SHR, 
    LT, LE, GT, GE,
    EQ, NEQ, 
    AND, XOR, OR,
    LAN, LOR,
    ASSIGN, MULASS, DIVASS, MODASS, ADDASS, SUBASS, SHLASS, SHRASS, ANDASS, XORASS, ORASS, COND, COLON,
    COMMA
};
TOK_TYPE tk = END;
struct Node;
char* start_pos = NULL;
char* next_pos = NULL;
TOK_TYPE next();
void consume();
void error();
void expect();
bool is_ternary(TOK_TYPE tok);
bool is_binary(TOK_TYPE tok);
bool is_unary(TOK_TYPE tok);
bool is_suffix(TOK_TYPE tok);
bool is_type(TOK_TYPE tok);
TOK_TYPE get_type(char* s, char* e);
void pushOperator(TOK_TYPE op, stack<TOK_TYPE>& operators, stack<Node*>& operands);
void popOperator(stack<TOK_TYPE>& operators, stack<Node*>& operands);
TOK_TYPE unary(TOK_TYPE token);
void P(stack<TOK_TYPE>& operators, stack<Node*>& operands);
void E(stack<TOK_TYPE>& operators, stack<Node*>& operands);
int precedence_level(TOK_TYPE tok);
bool precedent_over(TOK_TYPE lhs, TOK_TYPE rhs);
void print_prefix(Node* root);
void expr(stack<TOK_TYPE>& operators, stack<Node*>& operands);
struct Node{
    TOK_TYPE _type;
    vector<Node*>_sons;
    char* _value;
    int _n_sons;
    int num_sons(){
        return _n_sons;
    }
    bool has_sons(){
        return _n_sons;
    }
    void add_son(Node* son){
        if (son){
            _sons.push_back(son);
	        _n_sons ++;
	    }
    }
    void ncpy(char* start_pos, int length){
        _value = (char*)malloc(sizeof(char) * (length + 1));
        strncpy(_value, start_pos, length);
        *(_value + length) = '\0';
    }
    Node():_n_sons(0), _type(END), _value(NULL){
    
    }
    ~Node(){
        delete _value;
        for(int i = 0; i < _n_sons; ++i){
            delete _sons[i];
        }
    }
};
bool is_binary(TOK_TYPE tok){
     return  tok == FUNC || tok == COMMA || tok == IDX
         || tok == ADD || tok == SUB || tok == MUL || tok == DIV || tok == MOD|| tok == EXP
         || tok == SHL || tok == SHR || tok == LT || tok == GT || tok == GE
         || tok == LE || tok == EQ || tok == NEQ || tok == AND || tok == XOR 
         || tok == OR || tok == LAN || tok == LOR
         || tok == ASSIGN || tok == MULASS
         || tok == DIVASS || tok == MODASS || tok == ADDASS || tok == SUBASS || tok == SHLASS
         || tok == SHRASS || tok == ANDASS || tok == XORASS || tok == ORASS;
}
bool is_ternary(TOK_TYPE tok){
    return tok == COND;
}
bool is_unary(TOK_TYPE tok){
     return tok == NEG || tok == SUB /*? ?*/|| tok == POS || tok == PINC || tok == PDEC || tok == NOT || tok == LNOT
         || tok == TINT
         || tok == L_PAR// for func call
     || tok == L_BR;//for IDX TODO make a is_suffix function
}
bool is_suffix(TOK_TYPE tok){
     return tok == SINC || tok == SDEC;
}
bool is_type(TOK_TYPE tok){
    return tok == TINT;
}
TOK_TYPE get_type(char* s, char* e){
    if (*s == 'i' && *(s+1) == 'n' && *(s+2) == 't') return TINT;
    else return END;
}
bool precedent_over(TOK_TYPE lhs, TOK_TYPE rhs){
    int level_lhs = precedence_level(lhs);
    int level_rhs = precedence_level(rhs);
    if (level_lhs > level_rhs){
        return true;
    } else if (level_lhs == level_rhs){
       if (lhs == COND ||lhs == ASSIGN || lhs == MULASS
               ||lhs == TINT
         || lhs == DIVASS || lhs == MODASS || lhs == ADDASS || lhs == SUBASS || lhs == SHLASS
         || lhs == SHRASS || lhs == ANDASS || lhs == XORASS || lhs == ORASS
               || lhs == PINC || lhs == PDEC || lhs == NEG || lhs == END || lhs == NOT || lhs == LNOT) return false;//right associative
       else return true;//left associative
    } else {
        return false;
    }
}
int precedence_level(TOK_TYPE tok){
    if (tok == SINC || tok == SDEC || tok == FUNC || tok == IDX) return 5;
    else if (tok == PINC || tok == PDEC || tok == NEG || tok == POS || tok == LNOT || tok == NOT || tok == TINT) return 4;
    else if (tok == MUL || tok == DIV || tok == MOD) return 3;
    else if (tok == ADD || tok == SUB) return 1;
    else if (tok == SHL || tok == SHR) return 0;
    else if (tok == LE || tok == LT || tok == GT || tok == GE) return -1;
    else if (tok == EQ || tok == NEQ) return -2;
    else if (tok == AND) return -3;
    else if (tok == XOR) return -4;
    else if (tok == OR) return -5;
    else if (tok == LAN) return -6;
    else if (tok == LOR) return -7;
    else if (tok == COND || tok == ASSIGN || tok == MULASS
         || tok == DIVASS || tok == MODASS || tok == ADDASS || tok == SUBASS || tok == SHLASS
         || tok == SHRASS || tok == ANDASS || tok == XORASS || tok == ORASS
            ) return -8;
    else if (tok == COMMA) return -9;
    else return -100;
}
void consume(){
    start_pos = next_pos;
    tk = END;
}
void error(){
    printf("fucking error");
    exit(-1);
}
void expect(TOK_TYPE tok){
    TOK_TYPE next_tok = next();
    if (next_tok == tok)
	    consume();
    else
	    error();
}
Node* mkNode(TOK_TYPE token, Node* first, Node* second = NULL, Node* third = NULL){
    Node* root = new Node();
    root->_type = token;
    root->ncpy(start_pos, next_pos - start_pos);
    root->add_son(first);
    root->add_son(second);
    root->add_son(third);
    return root;
}
Node* mkLeaf(TOK_TYPE tok){
    Node* root = new Node();
    root->_type = tok;
    root->ncpy(start_pos, next_pos - start_pos);
    return root;
}
TOK_TYPE next(){
    if (start_pos != next_pos){
        return tk;
    }
    while(*start_pos == ' ') ++ start_pos;
    next_pos= start_pos;
    if (*next_pos >= '0' && *next_pos <= '9'){
        tk = INT_CONST;
        ++next_pos;
       while(*next_pos >= '0' && *next_pos <= '9') ++next_pos;
    } else if (*next_pos >= 'a' && *next_pos <= 'z' || *next_pos >= 'A' && *next_pos <= 'Z' || *next_pos == '_'){
        ++next_pos;
        while(*next_pos >= 'a' && *next_pos <= 'z' || *next_pos >= 'A' && *next_pos <= 'Z' || *next_pos == '_' || *next_pos >= '0' && *next_pos <= '9')++ next_pos;
        if ((tk = get_type(start_pos, next_pos)) == END) tk = ID;
    } else if (*next_pos == '\''){
        ++next_pos; tk = CHAR_CONST;
        if (*next_pos == '\\') {++next_pos;}++next_pos;
        if (*next_pos == '\''){ ++ next_pos; }else {error(/*"missing closing single quotion mark"*/);}
    } else if (*next_pos == '\"'){
        ++next_pos; tk = STRING;
        while (*next_pos != '\0' && *next_pos != '\"') { if (*next_pos == '\\'){++next_pos;}++next_pos;}
        if (*next_pos == '"'){ ++ next_pos; }else {error(/*"missing closing double quotion mark"*/);}
    } else if (*next_pos == '+'){
        if (*(next_pos + 1) == '+'){ next_pos += 2; tk = PINC;}else {++next_pos; tk = ADD;}
    } else if (*next_pos == '-'){
        if (*(next_pos + 1) == '-'){next_pos += 2; tk = PDEC;} else {++next_pos; tk = SUB;}
    } else if (*next_pos == '*'){
        if (*(next_pos + 1) == '='){next_pos +=2; tk = MULASS;}else {++next_pos; tk = MUL;}
    } else if (*next_pos == '/'){
        if (*(next_pos + 1) == '='){next_pos +=2; tk = DIVASS;}else {++next_pos; tk = DIV;}
    } else if (*next_pos == '%'){
        if (*(next_pos + 1) == '='){next_pos +=2; tk = MODASS;}else {++next_pos; tk = MOD;}
    } else if (*next_pos == '<') {
        if (*(next_pos + 1) == '<'){ next_pos += 2; tk = SHL;} else if (*(next_pos + 1) == '=') {next_pos += 2; tk = LE;} else {++next_pos; tk = LT;}
    } else if (*next_pos == '>') {
        if (*(next_pos + 1) == '>'){next_pos += 2; tk = SHR;} else if (*(next_pos + 1) == '=') { next_pos += 2; tk = GE;} else { ++next_pos; tk = GT;}
    } else if (*next_pos == '='){
        if (* (next_pos + 1) == '='){next_pos += 2; tk = EQ;} else {++next_pos; tk=ASSIGN;}
    } else if (*next_pos == '!'){
        if (*(next_pos + 1) == '='){next_pos += 2; tk = NEQ;} else {++next_pos; tk = LNOT;}
    } else if (*next_pos == '~'){ ++ next_pos; tk = NOT;
    } else if (*next_pos == '&'){
        if (*(next_pos + 1) == '&'){next_pos += 2; tk = LAN;} else {++next_pos; tk = AND;}
    } else if (*next_pos == '^'){++next_pos; tk = XOR;
    } else if (*next_pos == '|'){
        if (*(next_pos + 1) == '|'){next_pos += 2; tk = LOR;} else{++next_pos; tk = OR;}
    } else if (*next_pos == '('){++next_pos; tk = L_PAR;
    } else if (*next_pos == ')'){++next_pos; tk = R_PAR;
    } else if (*next_pos == '['){++next_pos; tk = L_BR;
    } else if (*next_pos == ']'){++next_pos; tk = R_BR;
    } else if (*next_pos == '?'){++next_pos; tk = COND;
    } else if (*next_pos == ':'){++next_pos; tk = COLON;
    } else if (*next_pos == ','){++next_pos; tk = COMMA;
    } else { tk = END; }
    return tk;
}
Node* Eparser(){
    stack<TOK_TYPE> operators;
    stack<Node*> operands;
    operators.push(END);
    expr(operators, operands);
    expect(END);
    return operands.top();
}
void expr(stack<TOK_TYPE>& operators, stack<Node*>& operands){
    P(operators, operands);

    while(is_binary(next()) || is_ternary(next())){
        auto next_tok = next();
        pushOperator(next(), operators, operands);
        consume();
        P(operators, operands);
        if (is_ternary(next_tok)){
            next();
            consume();
            P(operators, operands);
        }
    }
    while(operators.top() != END){
        popOperator(operators, operands);
    }
}
void P(stack<TOK_TYPE>& operators, stack<Node*>& operands){
    TOK_TYPE next_token = next();
    if (next_token == INT_CONST || next_token == ID  || next_token == CHAR_CONST || next_token == STRING /*|| is_keyword(next_token)*/){
        operands.push(mkLeaf(next_token));
        consume();
        while(is_unary(next())){ // postfix unary operator
            if (next() == PINC || next() == PDEC){
                tk = tk == PINC ? SINC : SDEC;
                pushOperator(unary(tk), operators, operands);
                consume();
            } else {
                while (next() == L_PAR || next() == L_BR){
                    if (next() == L_PAR){
                        pushOperator(FUNC, operators, operands);
                        consume();
                        if (next() == R_PAR){
                            operands.push(mkLeaf(END)); 
                            consume();
                        } else {
                            operators.push(END);
                            expr(operators, operands);
                            expect(R_PAR);
                           operators.pop();
                        }
                    } else {
                        pushOperator(IDX, operators, operands);
                        consume();
                        if (next() == R_BR){
                            operands.push(mkLeaf(END)); 
                            consume();
                        } else {
                            operators.push(END);
                            expr(operators, operands);
                            expect(R_BR);
                           operators.pop();
                        }
                    }
                }
            }
        }
    } else if (next_token  == L_PAR){
        consume();
        if (is_type(next())){
            pushOperator(next(), operators, operands);
            consume();
            expect(R_PAR);
            P(operators, operands);
        }else {
            operators.push(END);
            expr(operators, operands);
            expect(R_PAR);
            operators.pop();
        }
    } else if (is_unary(next_token)){//prefix unary operator
        pushOperator(unary(next_token), operators, operands);
        consume();
        while(is_unary(next())){
            pushOperator(unary(next()), operators, operands);
            consume();
        }
        P(operators, operands);
    } else {
        error();
    }
}
TOK_TYPE unary(TOK_TYPE token){// TODO check logic
    if (token == SUB) return NEG;
    else return token;
}
void popOperator(stack<TOK_TYPE>& operators, stack<Node*>& operands){
   if (is_binary(operators.top())) {
       auto t1 = operands.top(); operands.pop();
       auto t0 = operands.top(); operands.pop();
       operands.push(mkNode(operators.top(), t0, t1));
       operators.pop();
   } else if (is_ternary(operators.top())) {
       auto t2 = operands.top(); operands.pop();
       auto t1 = operands.top(); operands.pop();
       auto t0 = operands.top(); operands.pop();
       operands.push(mkNode(operators.top(), t0, t1, t2));
       operators.pop();
   } else {
       auto res = mkNode(operators.top(), operands.top());
       operands.pop(); operators.pop();
       operands.push(res);
   }
}
void pushOperator(TOK_TYPE op, stack<TOK_TYPE>& operators, stack<Node*>& operands){
    printf("%d %d\n", operators.size(), operands.size());
    auto to = operators.top();
    while(precedent_over(operators.top(), op)){
        popOperator(operators, operands);
    }
    operators.push(op);
}
void print_prefix(Node* root){
    if (!root) return;
    int num_sons = root->num_sons();
    switch(root->_type){
    case COMMA:
        break;
    case END:
        printf("NULL");
        break;
    case FUNC:
        printf("CALL", root->_value);
        break;
    case IDX:
        printf("[]");
        break;
    case TINT:
        printf("int");
        break;
    case STRING:
        printf("%s", root->_value);
        break;
    case CHAR_CONST:
        printf("%s", root->_value);
        break;
    case MUL:
		printf("*");
		break;
	case DIV:
		printf("/");
		break;
	case ADD:
		printf("+");
		break;
	case SUB:
		printf("-");
		break;
	case INT_CONST:
		printf("%s", root->_value);
		break;
    case ID:
        printf("%s", root->_value);
        break;
    case EXP:
        printf("^");
        break;
    case MOD:
        printf("%%");
        break;
    case NEG:
        printf("-");
        break;
    case SHL:
        printf("<<");
        break;
    case SHR:
        printf(">>");
        break;
    case LE:
        printf("<=");
        break;
    case LT:
        printf("<");
        break;
    case GE:
        printf(">=");
        break;
    case GT:
        printf(">");
        break;
    case EQ:
        printf("==");
        break;
    case NEQ:
        printf("!=");
        break;
    case AND:
        printf("&");
        break;
    case XOR:
        printf("^");
        break;
    case OR:
        printf("|");
        break;
    case LAN:
        printf("&&");
        break;
    case LOR:
        printf("||");
        break;
    case ASSIGN:
        printf("=");
        break;
    case MULASS:
        printf("*=");
        break;
    case DIVASS:
        printf("/=");
        break;
    case MODASS:
        printf("%%=");
        break;
    case ADDASS:
        printf("+=");
        break;
    case SUBASS:
        printf("-=");
        break;
    case SHLASS:
        printf("<=");
        break;
    case SHRASS:
        printf(">=");
        break;
    case ANDASS:
        printf("&=");
        break;
    case XORASS:
        printf("^=");
        break;
    case ORASS:
        printf("|=");
        break;
    case COND:
        printf("?:");
        break;
    case PINC:
        printf("++");
        break;
    case PDEC:
        printf("--");
        break;
    case SINC:
        printf("++");
        break;
    case SDEC:
        printf("--");
        break;
    case LNOT:
        printf("!");
        break;
    case NOT:
        printf("~");
        break;
    default:
        printf("\nfucking type [%d] error[%s]\n",root->_type, root->_value);
		break;
    }
    if (num_sons){
        printf("(");
	    print_prefix(root->_sons[0]);
        for(int i = 1; i < num_sons; ++i){
            printf(",");
            print_prefix(root->_sons[i]);
        }
	    printf(")");
    }
}
char input2[] = "a&b && c||d";
char input[] = "a^b*c<<d>> e   <= h == 5 & 3 ^ 4 | 5 && 9";
char input3[] = "3&&9";
char input4[] = "a=b=c";
char input5[] = "++----a";
char input6[] = "--a++";
char input7[] = "-a";
char input8[] = "!~a++--";
char input9[] = "c%=a%b";
char input10[] = "c = '\''";
char input11[] = "c = \"fuck your mother\"";
char input12[] = "(int)(int) a";
char input13[] = "fuck(a, b, c, d)";
char input14[] = "fuck[a, b, c, d]";

Node* root = nullptr;
int main(){
	start_pos = input14;
    next_pos = start_pos;
    root = Eparser();
	print_prefix(root);
}
