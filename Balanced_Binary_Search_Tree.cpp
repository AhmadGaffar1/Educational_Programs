//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// used libraries
#include<cstdio>
#include<cmath>
#include<queue>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// #defines & global variables
#define minAge 10
#define maxAge  1000

#define minRank 1
#define maxRank 2e9



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// personalCard class
class personalCard
{
public:
    personalCard(char* name, char* address, int* age, int* rank) : name(name), address(address), age(age), rank(rank)
    {
        personalNumber=ID++;
        name=address=nullptr;
        age= rank=nullptr;
    }

    ~personalCard()
    {
        delete name;
        delete address;
        delete age;
        delete rank;

        name=address=nullptr;
        age=rank=nullptr;
    }

    void print()
    {
        printf("==============================================\n");
        printf("Name :: %s\n",name);
        printf("Address :: %s\n",address);
        printf("Age :: %i\n\n",*age);
        printf("Rank :: %i\n\n",*rank);
        printf("Personal Number :: %i\n",personalNumber);
        printf("==============================================\n");
    }

    char* name;
    char* address;
    int* age;
    int* rank;
    int personalNumber;


    static int ID;
};

int personalCard::ID=1;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Balanced Binary Search Tree ( BBST or AVL ) class
struct NodeBBST
{
    // prefer initialize pointer before constructing object for maximize safety
    NodeBBST(const int& value, personalCard* card) : card(card) , left(nullptr) , right(nullptr)
    {
        length = 1;
        size = 1;

        this->value = value;
        smallest = value;
        largest = value;

        card = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    ~NodeBBST()
    {
        int tmp=*card->rank;
        printf("\nStart  Destructor Rank: %i\n",tmp); // debug ( compiler tracking )

        if(left!=nullptr)  delete left,  left=nullptr;
        if(right!=nullptr) delete right, right=nullptr;
        if(card!=nullptr)  delete card,  card=nullptr;

        printf("End  Destructor Rank: %i\n",tmp);}  // debug ( compiler tracking )


    int value, smallest, largest; // value ( value used for sorting tree ) , smallest ( smallest value in my sub tree ) , largest ( largest value in my sub tree )
    int length, size;             // length ( longest path in my sub tree ) , size ( size of my sub tree )

    personalCard* card;           // card ( this is complete card of my data )
    NodeBBST* left, * right;      // left ( left child ) , right ( right child )
};

class BBST
{
public:
    // prefer initialize pointer before constructing object for maximize safety
    BBST(bool master = 0) : master(master), head(nullptr), ptr(nullptr), ptr2(nullptr)
    {
        tmp = tmp2 = exist = insertedCount = deletedCount = level = 0;
    }

    ~BBST()
    {
        printf("Start BBST :: Destructor\n\n"); // debug ( compiler tracking )

        ptr=ptr2=nullptr;
        if(head!=nullptr) delete head, head=nullptr;

        printf("\nStart BBST :: Destructor\n\n"); // debug ( compiler tracking )
    }

    bool insertByValue(const int& value, personalCard* card)
    {
        prepare();
        exist = 1;

        NodeBBST* p= new NodeBBST(value, card);
        ptr=p;

        if (head != nullptr) head = insertByValue2(head);
        else                 head = ptr, exist = 0;

        if (!exist) ++insertedCount;
        else delete p;

        ptr = p = nullptr;
        card = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return !exist;
    }

    bool deleteByValue(const int& value)
    {
        prepare();
        exist = 0;
        if(head!=nullptr) head = deleteByValue2(head, value);

        if (exist) ++deletedCount;

        return exist;
    }

    personalCard* searchByValue(const int& value)
    {
        prepare();
        tmp = value;
        if(head!=nullptr) return searchByValue2();
        return nullptr;
    }

/*    void copiedKilled(const int& publicID)
    {
        prepare();
        ptr->card = searchByValue(publicID);

        if (ptr->card != nullptr) ptr->card->copied = 0;
    }*/



    void printTreeShape()
    {
        prepare();
        if(head!=nullptr)
        {
            level=0;

            q1.push(head);
            //q2 --> empty

            printTreeShape2();
            // printByLevel(); // can call this function outside   printTreeShape();
        }
        else printf("\nThis Tree Is Empty Now !!!\n");
    }

    void printByLevel()
    {
        prepare();
        if(head!=nullptr)
        {
            level=0;

            q1.push(head);
            //q2 --> empty

            printByLevel2();
        }
        else printf("\nThis Tree Is Empty Now !!!\n");
    }

    void printPrefixes()
    {
        prepare();
        if(head!=nullptr) printPrefixes2(head);
        else printf("\nThis Tree Is Empty Now !!!\n");
    }

    void printPostfixes()
    {
        prepare();
        if(head!=nullptr) printPostfixes2(head);
        else printf("\nThis Tree Is Empty Now !!!\n");
    }

    void printAscending()
    {
        prepare();
        if(head!=nullptr) printAscending2(head);
        else printf("\nThis Tree Is Empty Now !!!\n");
    }

    void printDescending()
    {
        printf("\n\n\nHAAAAAAAAAAAAAAAAAAAAAAAY Descending (-_-)\n\n\n");
        prepare();
        if(head!=nullptr) printDescending2(head);
        else printf("\nThis Tree Is Empty Now !!!\n");
    }

    void printRange(int& mini, int& maxi)
    {
        prepare();
        tmp = mini; tmp2 = maxi;
        if(tmp>tmp2) swap(tmp,tmp2);

        if(head!=nullptr) printRange2(head);
        else printf("\nThis Tree Is Empty Now !!!\n");
    }

    int length()
    {
        if(head==nullptr) return 0;
        return head->length;
    }

    int size()
    {
        if(head==nullptr) return 0;
        return head->size;
    }

    int smallestValue()
    {
        if(head!=nullptr) return head->smallest;
        else printf("\nThis Tree Is Empty Now !!!\n");
        return 0;
    }

    int largestValue()
    {
        if(head!=nullptr) return head->largest;
        else printf("\nThis Tree Is Empty Now !!!\n");
        return 0;
    }

    int inserted()
    {
        return insertedCount;
    }

    int deleted()
    {
        return deletedCount;
    }

    void clear()
    {
        if(master) delete head;
        head=nullptr;
    }

private:
    NodeBBST* head, * ptr, * ptr2;
    int tmp, tmp2;
    bool exist, master;
    int insertedCount, deletedCount;

    queue<NodeBBST*> q1,q2;
    int level;



    void prepare()
    {
        ptr = ptr2 = head;
        tmp = tmp2 = exist = 0;
    }



    NodeBBST* insertByValue2(NodeBBST* node)
    {
        if (ptr->value < node->value && node->left != nullptr)       node->left = insertByValue2(node->left);

        else if (ptr->value > node->value && node->right != nullptr) node->right = insertByValue2(node->right);

        else if (ptr->value < node->value && node->left == nullptr)  node->left = ptr, exist = 0;

        else if (ptr->value > node->value && node->right == nullptr) node->right = ptr, exist = 0;


        if(!exist) ptr=balanced(node);
        else       ptr=node;

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return ptr;
    }

    NodeBBST* deleteByValue2(NodeBBST* node, const int& value)
    {
        if (node->value == value)
        {
            if (node->left == nullptr && node->right == nullptr) ptr2 = nullptr;

            else if (len(node->left) >= len(node->right))
            {
                node->left = largest(node->left);

                ptr2->left = node->left;
                ptr2->right = node->right;
            }

            else if (len(node->left) < len(node->right))
            {
                node->right = smallest(node->right);

                ptr2->left = node->left;
                ptr2->right = node->right;
            }

            if (!master) node->card = nullptr;
            node->left = node->right = nullptr;
            delete node;

            exist = 1;
            node = ptr2;
        }

        else if (value < node->value && node->left!=nullptr)  node->left = deleteByValue2(node->left, value);
        else if (value > node->value && node->right!=nullptr) node->right = deleteByValue2(node->right, value);


        if(exist) ptr=balanced(node);
        else      ptr=node;

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return ptr;
    }

    personalCard* searchByValue2()
    {
        if (tmp == ptr->value) return ptr->card;

        else if (tmp < ptr->value && ptr->left != nullptr)  { ptr = ptr->left;  return searchByValue2(); }

        else if (tmp > ptr->value && ptr->right != nullptr) { ptr = ptr->right; return searchByValue2(); }

        else return nullptr;
    }

    NodeBBST* largest(NodeBBST* node)
    {
        if (node->right != nullptr) node->right = largest(node->right);

        else ptr2 = node, ptr = node->left, ptr2->left = nullptr,   node=ptr;


        ptr=balanced(node);

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return ptr;
    }

    NodeBBST* smallest(NodeBBST* node)
    {
        if (node->left != nullptr) node->left = smallest(node->left);

        else ptr2 = node, ptr = node->right, ptr2->right = nullptr, node = ptr;


        ptr = balanced(node);

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return ptr;
    }

    NodeBBST* balanced(NodeBBST* node)
    {
        ptr=node;

        if(ptr==nullptr) {node=nullptr; return ptr;}

        else if (len(node->left) - len(node->right) >= 2)
        {
            if (len(node->left->left) - len(node->left->right) >= 0)
            {
                ptr = node->left;
                node->left = ptr->right;
                ptr->right = node;
            }

            else
            {
                ptr = node->left->right;

                node->left->right = ptr->left;
                ptr->left = node->left;

                node->left = ptr->right;
                ptr->right = node;
            }
        }

        else if (len(node->left) - len(node->right) <= -2)
        {
            if (len(node->right->right) - len(node->right->left) >= 0)
            {
                ptr = node->right;
                node->right = ptr->left;
                ptr->left = node;
            }

            else
            {
                ptr = node->right->left;

                node->right->left = ptr->right;
                ptr->right = node->right;

                node->right = ptr->left;
                ptr->left = node;
            }
        }


        if(ptr!=nullptr && ptr->left!=nullptr)  update(ptr->left);
        if(ptr!=nullptr && ptr->right!=nullptr) update(ptr->right);
        if(ptr!=nullptr)                        update(ptr);

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return ptr;
    }

    int len(NodeBBST* node)
    {
        if (node == nullptr) tmp = 0;
        else                 tmp=node->length;

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return tmp;
    }

    void update(NodeBBST* node)
    {
        if(node->left!=nullptr && node->right!=nullptr)
        {
            node->size    = node->left->size + node->right->size+1;
            node->length  = max(node->left->length , node->right->length)+1;

            node->smallest= min(node->smallest , min(node->left->smallest , node->right->smallest));
            node->largest = max(node->largest , max(node->left->largest , node->right->largest));
        }

        else if(node->left!=nullptr && node->right==nullptr)
        {
            node->size    = node->left->size + 1;
            node->length  = node->left->length + 1;

            node->smallest= min(node->smallest , node->left->smallest);
            node->largest = max(node->largest , node->left->largest);
        }

        else if(node->left==nullptr && node->right!=nullptr)
        {
            node->size    = node->right->size + 1;
            node->length  = node->right->length + 1;

            node->smallest= min(node->smallest , node->right->smallest);
            node->largest = max(node->largest , node->right->largest);
        }

        else if(node->left==nullptr && node->right==nullptr)
        {
            node->size=1;
            node->length=1;

            node->smallest=node->value;
            node->largest=node->value;
        }

        node=nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }



    void printTreeShape2()
    {
        while(q1.size()) // shipping the level
        {
            q2.push(q1.front());

            q1.pop();
        }

        printf("Level[ %i ]: ",level++);

        while(q2.size()) // print the level , then shipping next level
        {
            for(int i=0;i<(head->length-level);++i) printf("\t");

            printf("(%i)",q2.front()->value);

            if(q2.front()->left!=nullptr && q2.front()->right!=nullptr) printf("-[%i,%i]",q2.front()->left->value,q2.front()->right->value);
            else if(q2.front()->left!=nullptr)  printf("-[%i, ]",q2.front()->left->value);
            else if(q2.front()->right!=nullptr) printf("-[ ,%i]",q2.front()->right->value);
            else printf("-[ , ]");


            if(q2.front()->left!=nullptr)  q1.push(q2.front()->left);
            if(q2.front()->right!=nullptr) q1.push(q2.front()->right);

            q2.pop();
        }

        printf("\n\n\n\n");

        if(q1.size()) printTreeShape2();
    }

    void printByLevel2()
    {
        while(q1.size()) // shipping the level
        {
            q2.push(q1.front());

            q1.pop();
        }

        printf("Level[ %i ] :: \n",level++);

        while(q2.size()) // print the level , then shipping next level
        {
            q2.front()->card->print();

            if(q2.front()->left!=nullptr)  q1.push(q2.front()->left);
            if(q2.front()->right!=nullptr) q1.push(q2.front()->right);

            q2.pop();
        }

        printf("\n\n\n");

        if(q1.size()) printByLevel2();
    }

    void printPrefixes2(NodeBBST* node)
    {
        node->card->print();
        printf("( Node SubTree Length: %i )\n",node->length);
        if (node->left != nullptr)  printPrefixes2(node->left);
        if (node->right != nullptr) printPrefixes2(node->right);

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void printPostfixes2(NodeBBST* node)
    {
        if (node->left != nullptr)  printPostfixes2(node->left);
        if (node->right != nullptr) printPostfixes2(node->right);
        node->card->print();
        printf("( Node SubTree Length: %i )\n",node->length);

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void printAscending2(NodeBBST* node)
    {
        if (node->left != nullptr)  printAscending2(node->left);
        node->card->print();
        if (node->right != nullptr) printAscending2(node->right);

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void printDescending2(NodeBBST* node)
    {
        if (node->right != nullptr) printDescending2(node->right);
        node->card->print();
        if (node->left != nullptr)  printDescending2(node->left);

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void printRange2(NodeBBST* node)
    {
        if (node->left != nullptr && node->left->largest >= tmp)     printRange2(node->left);
        if (node->value >= tmp && node->value <= tmp2)               node->card->print();
        if (node->right != nullptr && node->right->smallest <= tmp2) printRange2(node->right);

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// global functions for testing  -->  ( class Trie ) , ( class personalCard )
void readInteger(int* num, int a,int b)
{
    int ok=1;
    char* check=new char[100];

    printf("select in range ( %i : %i )  :: ",a,b);
    while(true)
    {
        *num=0; ok=1;

        scanf("%s",check);

        for(int i=0; check[i]!='\0' ;++i) if(check[i]<'0' || check[i]>'9') {ok=0; break;}

        if(ok) for(int i=0; check[i]!='\0' ;++i) *num=(*num*10) + (check[i]-'0');
        else
        {
            printf("\nplease enter only numbers !!!\n\nselect in range ( %i : %i )  :: ",a,b);
            continue;
        }

        if(*num>=a && *num<=b) break;
        else printf("\nplease select in  range  ( %i : %i )  :: ",a,b);
    }

    delete[] check;
    check=nullptr;
}

void readChar(char* STR)
{
    // continue reading any character until user push ENTER --> ( \n )

    int i=0;

    for (i = 0;i==0 || STR[i - 1] != '\n'; ++i) scanf("%c", &STR[i]);
    STR[i-1]='\0';

    for(i=0;STR[i]!='\0';++i) if(STR[i]>='A' && STR[i]<='Z') STR[i]+=('a'-'A');

    STR=nullptr;
}

void interestedPerson(BBST* tree,bool flag)
{
    int* rank=new int;

    printf("----------------------------------------------\n");
    while(true)
    {
        printf("\nInterested Rank: ");
        readInteger(rank,minRank,maxRank);

        if(tree->searchByValue(*rank)!=nullptr)
        {
            if(flag) {tree->deleteByValue(*rank);          printf("Successful Remove Person Process  (-_-)\n");}
            else     {tree->searchByValue(*rank)->print(); printf("Successful Search Person Process  (-_-)\n");}
            break;
        }

        else if(tree->size())
        {
            printf("\nPerson With This Rank Not Exist In This Tree Store !!!\n\n"
                          "[ 0 ] for return previous step\n"
                          "[ 1 ] for helping by print( Smallest Rank : Largest Rank )\n"
                          "[ 2 ] for try again without any helping\n\n");

            readInteger(rank,0,2);
            if(*rank==0) break;
            else if(*rank==1) printf("\nspecial range ( %i , %i )\n",tree->smallestValue(),tree->largestValue());
        }

        else printf("\nThis Tree Is Empty Now !!!\n");
    }
    printf("----------------------------------------------\n");

    delete rank;
    rank=nullptr;

    tree=nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
}

void addPerson(BBST* tree)
{
    char* name=new char[100];
    char* address=new char[100];
    int* age=new int;
    int* rank=new int;

    char useless; // useless: using to handle last ( \n ) from user before read next string
    int i=0;

    for(i=0;i<100;++i) name[i]='\0', address[i]='\0';

    printf("----------------------------------------------\n");

    printf("\nName: "); // allow duplicate name
    scanf("%c",&useless);
    readChar(name);

    printf("\nAddress: "); // allow duplicate address
    readChar(address);


    printf("\nAge: "); // allow duplicate age
    readInteger(age,minAge,maxAge);

    printf("\nRank: "); // no allow duplicate Rank
    while(true)
    {
        readInteger(rank,minRank,maxRank);
        if(!tree->searchByValue(*rank)) break;
        else printf("\nThis Rank Is Full Of !!! , please enter another Rank\n\n");
    }

    printf("----------------------------------------------\n");

    printf("Successful Add Person Process  (-_-)\n\n\n");


    personalCard* card=new personalCard(name,address,age,rank);

    tree->insertByValue(*card->rank, card);


    name=address=nullptr;
    age=rank=nullptr;

    card=nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    tree=nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
}

void show(int* option, int* a, int* b)
{
    printf("----------------------------------------------\n");
    printf("[ 1 ] for print prefix\n"
                  "[ 2 ] for print postfix\n\n"
                  "[ 3 ] for print ascending\n"
                  "[ 4 ] for print descending\n\n"
                  "[ 5 ] for print in range ( A : B )\n");
    printf("----------------------------------------------\n\n");

    readInteger(option,1,5);

    if(*option==5)
    {
        printf("\n( A ): ");
        readInteger(a,1,2e9);

        printf("\n( B ): ");
        readInteger(b,1,2e9);
    }
}

void test(BBST* tree)
{
    int* option=new int, *a=new int, *b=new int;

    while(true)
    {
        printf("----------------------------------------------\n");
        printf("[ 0 ] for exit whole program\n\n"
                      "[ 1 ] for add new person\n"
                      "[ 2 ] for search exit person\n"
                      "[ 3 ] for remove exit person\n\n"
                      "[ 4 ] for print count members\n"
                      "[ 5 ] for print Tree\n");
        printf("----------------------------------------------\n\n");

        readInteger(option,0,5);

        if(*option==0) break;
        else if(*option==1) addPerson(tree);
        else if(*option==2) interestedPerson(tree,0);
        else if(*option==3) interestedPerson(tree,1);
        else if(*option==4) printf("\nTree Size: %i\n\n",tree->size());

        else
        {
            show(option,a,b);

            printf("\n");
            tree->printTreeShape();
            printf("\n");

            if(*option==1) tree->printPrefixes();
            else if(*option==2) tree->printPostfixes();
            else if(*option==3) tree->printAscending();
            else if(*option==4) tree->printDescending();
            else if(*option==5) tree->printRange(*a,*b);
            printf("\n\n\n");
        }
    }

    printf("\n\n\t\tSuccessful ( SHUT DOWN )\n\n");

    delete option;
    delete a;
    delete b;
    option=a=b=nullptr;

    tree=nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// main()
int main()
{
    BBST tree(1);
    test(&tree);

    // this self balancing tree sorting data by ( Rank )
    // Creation condition ( Unique Rank )

    return 0;
}
