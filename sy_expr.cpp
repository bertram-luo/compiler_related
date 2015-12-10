#include <stdlib.h>
#include <stdio.h>
#include <stack>
using std::stack;
//  exp := assignment_exp {, assignment_exp}
//  assignment_exp := condition_exp | unary_exp assignment_operator assignment_exp
//  assignment_operator := '=' | '*=' | '/=' | '%=' | '+=' | '-=' || '<<=' || '>>=' || '&=' || '^=' | '|='
//  conditional_exp := logical_or_exp | logical_or_exp '?' exp ':' conditional_exp
//  const_exp := conditional_exp
//  logical_or_exp := logical_and_exp {{ '||' logical_and_exp}}
//  logical_and_exp := inclusive_or_ex {'&&' inclusive_or_exp}
//  inclusive_or_exp := exclusive_or_exp {'|' exclusive_or_exp}
//  exclusive_or_exp := and_exp {'^' and_exp}
//  and_exp := equality_exp {'&' equality_exp}
//  equality_exp := relational_exp {('!='|'==') relational_exp}
//  relational_exp := shift_expression {('<' | '>' | '<=' | '>=') shift_expression}
//  shift_expression := additive_exp {('<<'| '>>') additive_exp}
//  additive_expression := mult_exp {('+'|'-') mult_exp}
//  mult_exp := cast_exp {('*' | '/' | '%') cast_exp}
//  cast_exp : unary_exp | '(' type_name ')' cast_exp
//  unary_exp := postfix_exp | '++' unary_exp | '--' unary_exp | unary_operator cast_exp | 'sizeof' unary_exp | 'sizeof' '(' type_name ')'
//  unary_operator := '&' | '*' | '+' | '-' | '~' | '!'
//  postfix_exp := primary_exp {'[' exp ']' | '(' ')' | '(' argument_exp_list ')' | '.' id | '->' id | '++' | '--'}
//  primary_expression := id | const | string | '(' exp ')'
//  argument_exp_list := assignment_exp {{, assignment_exp}}
//  const := int_const | char_const | float_const | enumeration_const
//
//
enum TOK_TYPE {END, L_PAR, R_PAR, POS, NEG, INC, DEC, INT, ID, ADD, SUB, MUL, DIV, EXP, SHL, SHR, LT, LE, GT, GE, EQ, NEQ, AND, XOR, OR, LAN, LOR, ASSIGN};
TOK_TYPE tk = END;
struct Node;
char* start_pos = NULL;
char* next_pos = NULL;
TOK_TYPE next();
void consume();
void error();
void expect();
bool is_binary(TOK_TYPE tok);
bool is_unary(TOK_TYPE tok);
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
    Node* _left;
    Node* _right;
    char* _value;
    int _n_sons;
    int num_sons(){
        return _n_sons;
    }
    bool has_sons(){
        return _n_sons;
    }
    void add_left(Node* left){
        if (left){
	        _left = left;
	        _n_sons ++;
	    }
    }
    void add_right(Node* right){
	    if (right){
            _right = right;
	        _n_sons ++;
	    }
    }
    void ncpy(char* start_pos, int length){
        _value = (char*)malloc(sizeof(char) * (length + 1));
        strncpy(_value, start_pos, length);
        *(_value + length) = '\0';
    }
    Node():_n_sons(0), _type(END), _value(NULL), _left(NULL), _right(NULL){
    
    }
};
TOK_TYPE next(){
    if (start_pos != next_pos){
        return tk;
    }
    while(*start_pos == ' ') ++ start_pos;
    next_pos= start_pos;
    if (*next_pos >= '0' && *next_pos <= '9'){
        tk = INT;
        ++next_pos;
       while(*next_pos >= '0' && *next_pos <= '9') ++next_pos;
    } else if (*next_pos >= 'a' && *next_pos <= 'z' || *next_pos >= 'A' && *next_pos <= 'Z' || *next_pos == '_'){
        tk = ID;
        ++next_pos;
        while(*next_pos >= 'a' && *next_pos <= 'z' || *next_pos >= 'A' && *next_pos <= 'Z' || *next_pos == '_' || *next_pos >= '0' && *next_pos <= '9') ++ next_pos;
    } else if (*next_pos == '+'){
        if (*(next_pos + 1) == '+'){ next_pos += 2; tk = INC;}else {++next_pos; tk = ADD;}
    } else if (*next_pos == '-'){
        if (*(next_pos + 1) == '-'){next_pos += 2; tk = DEC;} else {++next_pos; tk = SUB;}
    } else if (*next_pos == '*'){++next_pos; tk = MUL;
    } else if (*next_pos == '/'){++next_pos; tk = DIV;
    } else if (*next_pos == '<') {
        if (*(next_pos + 1) == '<'){ next_pos += 2; tk = SHL; } else if (*(next_pos + 1) == '=') {next_pos += 2; tk = LE;} else {++next_pos; tk = LT;}
    } else if (*next_pos == '>') {
        if (*(next_pos + 1) == '>'){next_pos += 2; tk = SHR;} else if (*(next_pos + 1) == '=') { next_pos += 2; tk = GE;} else { ++next_pos; tk = GT;}
    } else if (*next_pos == '='){
        if (* (next_pos + 1) == '='){next_pos += 2; tk = EQ;} else {++next_pos; tk=ASSIGN;}
    } else if (*next_pos == '!'){
        if (*(next_pos + 1) == '='){next_pos += 2; tk = NEQ;} else {++next_pos; tk = END;}
    } else if (*next_pos == '&'){
        if (*(next_pos + 1) == '&'){next_pos += 2; tk = LAN;} else {++next_pos; tk = AND;}
    } else if (*next_pos == '^'){++next_pos; tk = XOR;
    } else if (*next_pos == '|'){
        if (*(next_pos + 1) == '|'){next_pos += 2; tk = LOR;} else{++next_pos; tk = OR;}
    } else if (*next_pos == '('){++next_pos; tk = L_PAR;
    } else if (*next_pos == ')'){++next_pos;tk = R_PAR;
    } else { tk = END; }
    return tk;
}
void consume(){
    start_pos = next_pos;
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
bool is_binary(TOK_TYPE tok){
     return tok == ADD || tok == SUB || tok == MUL || tok == DIV || tok == EXP || tok == SHL || tok == SHR || tok == LT || tok == GT || tok == GE || tok == LE || tok == EQ || tok == NEQ || tok == AND || tok == XOR || tok == OR || tok == LAN || tok == LOR || tok == ASSIGN;
}
bool is_unary(TOK_TYPE tok){
     return tok == NEG || tok == SUB || tok == POS || tok == INC || tok == DEC;
}
Node* mkNode(TOK_TYPE token, Node* first, Node* second = NULL){
    Node* root = new Node();
    root->_type = token;
    root->ncpy(start_pos, next_pos - start_pos);
    root->add_left(first);
    root->add_right(second);
    return root;
}
Node* mkLeaf(TOK_TYPE tok){
    Node* root = new Node();
    root->_type = tok;
    root->ncpy(start_pos, next_pos - start_pos);
    return root;
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
    while(is_binary(next())){
        pushOperator(next(), operators, operands);
        consume();
        P(operators, operands);
    }
    printf("%d %d %d\n", operators.size(), operands.size(), operators.top());
    while(operators.top() != END){
        popOperator(operators, operands);
    }
}
void P(stack<TOK_TYPE>& operators, stack<Node*>& operands){
    TOK_TYPE next_token = next();
    if (next_token == INT || next_token == ID){
        operands.push(mkLeaf(next_token));
        consume();
    } else if (next_token  == L_PAR){
        consume();
        operators.push(END);
        expr(operators, operands);
        expect(R_PAR);
        operators.pop();
    } else if (is_unary(next_token)){
        pushOperator(unary(next_token), operators, operands);
        consume();
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
       auto t1 = operands.top();
       operands.pop();
       auto t0 = operands.top();
       operands.pop();
       operands.push(mkNode(operators.top(), t0, t1));
       operators.pop();
   } else {
       auto res = mkNode(operators.top(), operands.top());
       operands.pop();
       operators.pop();
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
bool precedent_over(TOK_TYPE lhs, TOK_TYPE rhs){
    int level_lhs = precedence_level(lhs);
    int level_rhs = precedence_level(rhs);
    if (level_rhs == 2){ // unary
        return false;
    }
    if (level_lhs > level_rhs){
        return true;
    } else if (level_lhs == level_rhs){
       if (lhs == EXP || lhs == ASSIGN || lhs == INC || lhs == DEC || lhs == NEG || lhs == END) return false;
       else return true;
    } else {
        return false;
    }
}
int precedence_level(TOK_TYPE tok){
    if (tok == INC || tok == DEC) return 4;
    else if (tok == MUL || tok == DIV) return 3;
    else if (tok == NEG) return 2;
    else if (tok == ADD || tok == SUB) return 1;
    else if (tok == SHL || tok == SHR) return 0;
    else if (tok == LE || tok == LT || tok == GT || tok == GE) return -1;
    else if (tok == EQ || tok == NEQ) return -2;
    else if (tok == AND) return -3;
    else if (tok == XOR) return -4;
    else if (tok == OR) return -5;
    else if (tok == LAN) return -6;
    else if (tok == LOR) return -7;
    else if (tok == ASSIGN) return -8;
    else return -100;
}
void print_prefix(Node* root){
    if (!root) return;
    int num_sons = root->num_sons();
    switch(root->_type){
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
	case INT:
		printf("%s", root->_value);
		break;
    case ID:
        printf("%s", root->_value);
        break;
    case EXP:
        printf("^");
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
    case INC:
        printf("++");
        break;
    case DEC:
        printf("--");
        break;
    default:
        printf("\nfucking type [%d] error[%s]\n",root->_type, root->_value);
		break;
    }
    if (num_sons){
        printf("(");
	    print_prefix(root->_left);
    }
    if (num_sons > 1){
        printf(",");
        print_prefix(root->_right);
    }
    if (num_sons){
	    printf(")");
    }
}
char input2[] = "a&b && c||d";
char input[] = "a^b*c<<d>> e   <= h == 5 & 3 ^ 4 | 5 && 9";
char input3[] = "3&&9";
char input4[] = "a=b=c";
char input5[] = "++----a";

int main(){
	start_pos = input5;
    next_pos = start_pos;
	print_prefix(Eparser());
}
