//http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
//precedence
//()
//^
//* /
//- neg
//+ - 
#include <stdlib.h>
#include <stdio.h>
#include <stack>

using std::stack;

enum TOK_TYPE {END, INT, ID, ADD, SUB, MUL, DIV, EXP, NEG, L_PAR, R_PAR};
TOK_TYPE cur_token = END;
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
        return cur_token;
    }
    while(*start_pos == ' ') ++ start_pos;
    next_pos= start_pos;
    if (*next_pos >= '0' && *next_pos <= '9'){
       cur_token = INT;
        ++next_pos;
       while(*next_pos >= '0' && *next_pos <= '9') ++next_pos;
    } else if (*next_pos >= 'a' && *next_pos <= 'z' || *next_pos >= 'A' && *next_pos <= 'Z' || *next_pos == '_'){
        cur_token = ID;
        ++next_pos;
        while(*next_pos >= 'a' && *next_pos <= 'z' || *next_pos >= 'A' && *next_pos <= 'Z' || *next_pos == '_' || *next_pos >= '0' && *next_pos <= '9') ++ next_pos;
    } else if (*next_pos == '+'){
        cur_token = ADD;
        ++next_pos;
    } else if (*next_pos == '-'){
        cur_token = SUB;
        ++next_pos;
    } else if (*next_pos == '*'){
        cur_token = MUL;
        ++next_pos;
    } else if (*next_pos == '/'){
        cur_token = DIV;
        ++next_pos;
    } else if (*next_pos == '^'){
        cur_token = EXP;
        ++next_pos;
    } else if (*next_pos == '('){
        cur_token = L_PAR;
        ++next_pos;
    } else if (*next_pos == ')'){
        cur_token = R_PAR;
        ++next_pos;
    } else if (*next_pos == '\0'){
        cur_token = END;
    }
    return cur_token;
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
     return tok == ADD || tok == SUB || tok == MUL || tok == DIV || tok == EXP;
}
bool is_unary(TOK_TYPE tok){
     return tok == NEG || tok == SUB;
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
    E(operators, operands);
    expect(END);
    return operands.top();
}
void E(stack<TOK_TYPE>& operators, stack<Node*>& operands){
    P(operators, operands);
    while(is_binary(next())){
        pushOperator(next(), operators, operands);
        consume();
        P(operators, operands);
    }
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
        E(operators, operands);
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
TOK_TYPE unary(TOK_TYPE token){
    if (token == SUB) return NEG;
    else return END;
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
    while(precedent_over(operators.top(), op)){
        popOperator(operators, operands);
    }
    operators.push(op);
}
bool precedent_over(TOK_TYPE lhs, TOK_TYPE rhs){
    int level_lhs = precedence_level(lhs);
    int level_rhs = precedence_level(rhs);
    if (level_rhs == 1){
        return false;
    }
    if (level_lhs > level_rhs){
        return true;
    } else if (level_lhs == level_rhs){
       if (lhs == EXP) return false;
       else return true;
    } else {
        return false;
    }
}
int precedence_level(TOK_TYPE tok){
    if (tok == EXP) return 3;
    else if (tok == MUL || tok == DIV) return 2;
    else if (tok == NEG) return 1;
    else if (tok == ADD || tok == SUB) return 0;
    else return -1;
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
	    printf(")");
    }
}
char input[] = "a^b*c^d+e^f/g^(h+i)";
char input2[] = "a^-b*c^d+e^f/g^(h+i)";
char input3[] = "-a^-b";
int main(){
	start_pos = input3;
    next_pos = start_pos;
	print_prefix(Eparser());
}
