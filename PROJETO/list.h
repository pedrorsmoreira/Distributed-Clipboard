typedef struct _node{
	int fd; 
	struct _node *next;
}node; 

node * new_clipboard(int fd_);
void insert_clipboard(node *c1, node *c2); 
void rm_clipboard(node *c1); 
