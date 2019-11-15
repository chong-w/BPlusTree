#include<stdio.h>
#include<malloc.h>
#include<string.h>
#define T 4              
#define keyType int
#define valueType int


typedef struct BPNode
{
	int is_leaf;         
	int num_key;
	keyType key[T];
	valueType  value[T];
	struct BPNode* child[T];
	struct BPNode* bro;
	struct BPNode* parent;
}BPNode;

typedef struct BPTree
{
	BPNode* root;
	BPNode* start;
	char name[100];
	FILE* fp;
}BPTree;


int initial(BPTree* Tree){

	// for(int i=0;i<4;i++){
	// 	P_BPNode node=(P_BPNode)malloc(sizeof(BPNode));
	// 	node->is_leaf=structFile[i][0];
	// 	node->n=structFile[i][1];
	// 	for(int j=0;j<3;j++){
	// 		node->key[j]=structFile[i][2*j+2];
	// 		node->child[j]=structFile[i][2*j+3];
	// 	}
	// 	node->next=structFile[i][8];
	// 	nodes[i]=node;
	// }
	

	return 0;
}


int find(BPTree* Tree,keyType key){
	BPNode* temp=Tree->root;
	while(!temp->is_leaf){
		int i=0;
		while(i<temp->num_key&&key>=temp->key[i]){
			i++;
		}
		if(i==0){
			return -1;
		}else{
			temp=temp->child[i-1];
		}
	}
	int j=0;
	while(j<temp->num_key&&key!=temp->key[j]){
		j++;
	}
	if(j<temp->num_key){
		return temp->value[j];
	}else{
		return -1;
	}
}

BPNode* leaf_split(BPNode* temp, keyType key, valueType value){
	//先把所有key、value排序暂存起来
	keyType keys[T+1];
	valueType values[T+1];
	//先把temp节点的k\v复制到ks\vs
	for(int i=0;i<T;i++){
		keys[i]=temp->key[i];
		values[i]=temp->value[i];
	}
	//新key/value插入
	for(int i=T;i>0;i--){
		if(keys[i-1]>key){
			keys[i]=keys[i-1];
			values[i]=values[i-1];
		}else{
			keys[i]=key;
			values[i]=value;
			break;
		}
	}
	//分裂
	BPNode* newnode=(BPNode*)malloc(sizeof(BPNode));
	for(int i=0;i<=T/2;i++){
		newnode->key[i]=keys[i+2];
		newnode->value[i]=values[i+2];
	}
	for(int i=0;i<T/2;i++){
		temp->key[i]=keys[i];
		temp->value[i]=values[i];
	}
	//其他一些设置
	temp->num_key=T/2;
	newnode->num_key=T/2+1;
	newnode->bro=temp->bro;
	temp->bro=newnode;
	newnode->is_leaf=1;

	return newnode;
	
}

BPNode* branch_split(BPNode* parent, BPNode* newnode){
	// 新节点在父节点的key就是该节点的第一个key
	keyType key=newnode->key[0];
	//先把所有key、child排序暂存起来
	keyType keys[T+1];
	BPNode* childs[T+1];
	//先把parent节点的k\c复到ks\cs
	for(int i=0;i<T;i++){
		keys[i]=parent->key[i];
		childs[i]=parent->child[i];
	}

	//新key/child插入
	int i; //记录child插入新/旧节点
	for(i=T;i>0;i--){
		if(keys[i-1]>key){
			keys[i]=keys[i-1];
			childs[i]=childs[i-1];
		}else{
			keys[i]=key;
			childs[i]=newnode;
			break;
		}
	}
	//分裂
	BPNode* newparent=(BPNode*)malloc(sizeof(BPNode));
	for(int i=0;i<=T/2;i++){
		newparent->key[i]=keys[i+2];
		newparent->child[i]=childs[i+2];
	}
	for(int i=0;i<T/2;i++){
		parent->key[i]=keys[i];
		parent->child[i]=childs[i];
	}
	//其他一些设置
	parent->num_key=T/2;
	newparent->num_key=T/2+1;
	newparent->is_leaf=0;
	newnode->parent=(i<T/2?parent:newparent);

	return newparent;
	
}

int new_root(BPTree* Tree,BPNode* root,BPNode* newnode){

	BPNode* newroot=(BPNode*)malloc(sizeof(BPNode));
	newroot->is_leaf=0;
	newroot->num_key=2;
	newroot->parent=NULL;
	newnode->parent=newroot;root->parent=newroot;
	newroot->child[0]=root;newroot->child[1]=newnode;
	newroot->key[0]=root->key[0];newroot->key[1]=newnode->key[0];
	Tree->root=newroot;
	return 0;
}

int branch_insert(BPTree* Tree,BPNode* temp, BPNode* newnode){
	//这里temp是child分裂剩余的节点
        BPNode* q=NULL;	
	BPNode* p=temp;
	BPNode* parent=p->parent;
	while(parent!=NULL){
		if(parent->num_key<T){
			//父节点未满，找合适的地方插入
			int i=parent->num_key;
			keyType key=newnode->key[0];
			while(i>0&&parent->key[i-1]>key){
				parent->key[i]=parent->key[i-1];
				parent->child[i]=parent->child[i-1];
				i--;
			}
			parent->key[i]=key;
			parent->child[i]=newnode;
			parent->num_key++;
			newnode->parent=parent;
			return 0;
		}else{
			//父节点已满,分裂产生父节点同级节点
			q=branch_split(parent,newnode);
			//新的节点向上插入父节点
			p=parent;
			newnode = q;
			parent=p->parent;
		}

	}
	//到达根节点了，需要生成新的根
		return new_root(Tree,p,newnode);
}


int insert(BPTree* Tree, keyType key,valueType value){

	BPNode* temp=Tree->root;
	// 向下找到要插入的叶子节点
	while(!temp->is_leaf){
		int i=0;
		while(i<temp->num_key&&key>=temp->key[i]){
			i++;
		}
		if(i==0){
			temp=temp->child[0];
		}else{
			temp=temp->child[i-1];
		}
	}
	//此时temp指向应该插入的叶子节点
	if(temp->num_key<T){
		//叶子点未满，找合适的地方插入
		int i=temp->num_key;
		while(i>0&&temp->key[i-1]>key){
			temp->key[i]=temp->key[i-1];
			temp->value[i]=temp->value[i-1];
			i--;
		}
		temp->key[i]=key;
		temp->value[i]=value;
		temp->num_key++;
		return 0;
	}else{
		//叶子点已满,分裂
		BPNode* newleaf=leaf_split(temp,key,value);
		return branch_insert(Tree,temp,newleaf);
	}
}


int update(BPTree* Tree, keyType key, valueType value){

	BPNode* temp=Tree->root;
	while(!temp->is_leaf){
		int i=0;
		while(i<temp->num_key&&key>=temp->key[i]){
			i++;
		}
		if(i==0){
			return -1;
		}else{
			temp=temp->child[i-1];
		}
	}
	int j=0;
	while(j<temp->num_key&&key!=temp->key[j]){
		j++;
	}
	if(j<temp->num_key){
		temp->value[j]=value;
	}else{
		return -1;
	}
	return 0;
}

int delete(BPTree* Tree, keyType key){
	

	return 0;
}


void main(){

	keyType keys[14]={5,6,7,8,9,10,15,16,17,18,19,20,21,22};
	valueType values[14]={0,1,2,3,4,5,6,7,8,9,10,11,12,13};

	BPNode* root=(BPNode*)malloc(sizeof(BPNode));
	root->is_leaf=1;
	root->num_key=0;
	root->parent=NULL;
	root->bro=NULL;

	BPTree* tree=(BPTree*)malloc(sizeof(BPTree));
	tree->root=root;
	tree->start=root;

	for(int i=0;i<14;i++){
		insert(tree,keys[i],values[i]);
	}

	while(1){
		int key;
		printf("please input key:");
		scanf("%d",&key);
		valueType value=find(tree,key);
		printf("value:%d\n",value);
	}

}
