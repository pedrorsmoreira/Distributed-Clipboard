node * new_clipboard(int fd_)
{
	node *n = (node*) malloc (sizeof(node));
	n->next=NULL; 
	n->fd=fd_; 
	return(n); 
}

void insert_clipboard(node *c1, node *c2)
{
	c1->next=c2; 
}

void rm_clipboard(node *c1)
{
	node *aux;

	if (c1->next == NULL)
	{
		
	}
	else
	{
		aux=c1->next->next; 
		free(c1->next);
		c1->next=aux;
	} 
}

//funcao de free 