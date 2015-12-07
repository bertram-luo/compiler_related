#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// expr ::= term {{ ( '+' | '-' ) term}};
// term ::= factor {{ ('*'|'/') factor}} ;
// factor ::= literal | variable | (expr);
//  literal = [0-9] | [1-9][0-9]*
//
char input[]="(124+12)*7";
enum NType{NTYPE, MUL, ADD, SUB, DIV, INT, FLOAT, EXPR};
struct Node{
    NType _type;
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
    Node():_n_sons(0), _type(NTYPE), _value(NULL), _left(NULL), _right(NULL){
    
    }
};

char* cur = input;
Node* expr();
Node* factor(){
    if (*cur == '('){
	    ++cur;
            Node* fact = expr();
	    if (*cur == ')'){
	        ++cur;
	    }else{
		printf("error: missing right )\n");
		exit(1);
	    }
	    return fact;
    } else {
        char* ahead = cur;
        while(*ahead>= '0' && *ahead<= '9') ++ahead;
        Node* fact = new Node();
        fact->_type = INT;
        fact->_value = (char*)malloc(sizeof(char) * (ahead - cur + 1));
        strncpy(fact->_value, cur, ahead - cur);
        *(fact->_value + (ahead - cur)) = '\0';
	cur = ahead;
	return fact;
    }
}
Node* term(){
    Node* root = factor();
    while (*cur == '*' || *cur == '/'){
	    Node* new_root = new Node();
	    new_root->_type = *cur == '*' ? MUL : DIV;
	    ++cur;
	    new_root->add_left(root);
	    new_root->add_right(factor());
	    root = new_root;
    }
    return root;
}
Node* expr(){
	if(*cur){
		Node* root = term();
		while(*cur == '+' || *cur == '-'){
		    Node* new_root = new Node();
		    new_root->_type = *cur == '+' ? ADD : SUB;
		    ++cur;
		    new_root->add_left(root);
		    new_root->add_right(term());
		    root = new_root;
		}
		return root;
	} else {
	    return NULL;
	}
}
Node* stmt(){
    return expr();
}
Node* parse(){
   if (*cur){
       return stmt();
   } else {
       return NULL;
   }
}
void print(Node* root){
    if (!root) return;
    int num_sons = root->num_sons();
    if (num_sons){
        printf("(");
    }
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
	default:
		break;
    }
    printf("\n");
    if (num_sons){
	print(root->_left);
    }
    if (num_sons > 1){
        print(root->_right);
	printf(")\n");
    }
}
int main(){
    print(parse());
}