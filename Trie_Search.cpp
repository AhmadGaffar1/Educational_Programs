//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// used libraries
#include<cstdio>
#include<vector>
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



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Trie class
struct NodeTrie
{
    // prefer initialize pointer before constructing object for maximize safety
    NodeTrie(const char& data, const int& level, const char* prefix) : card(nullptr)
    {
        this->data = data;
        this->level= level + 1;

        if(this->level>0)
        {
            this->prefix=new char[this->level+1];

            for(int i=0;i<this->level-1;++i) this->prefix[i]=prefix[i];

            this->prefix[this->level-1]=data;

            this->prefix[this->level]='\0';
        }
        else this->prefix=nullptr;

        dataRepeated = 0;
        prefix=nullptr;
    }

    ~NodeTrie()
    {
        for (int i = 0; i < child.size(); ++i) delete child[i], child[i] = nullptr;
        child.clear();
        if(card!=nullptr) delete card, card=nullptr;
    }

    char data;
    char* prefix;
    int level;
    int dataRepeated;
    vector<NodeTrie*> child;
    personalCard* card;
};

class Trie
{
public:
    Trie(bool master = 0) : master(master)
    {
        head = new NodeTrie(' ',-1,nullptr);
        ptr = ptr2 = head;
        i = j = start = end = mid = exist =exist2 = insertedCount = deletedCount = level = 0;
    }

    ~Trie()
    {
        ptr=ptr2=nullptr;
        if(head!=nullptr) delete head, head=nullptr;
    }

    bool insertByName(char* name, personalCard* card)
    {
        prepare();
        this->name = name;
        this->card = card;

        exist = 1;
        insertByName2(head);
        if (!exist) ++insertedCount;

        this->name = name = nullptr;
        this->card = card = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return !exist;
    }

    bool deleteByName(char* name)
    {
        prepare();
        this->name = name;

        exist = 0;
        deleteByName2(head);
        if (exist) head->dataRepeated--, ++deletedCount;

        this->name = name = nullptr;
        return exist;
    }

    personalCard* searchByName(char* name)
    {
        prepare();
        for (i = 0; name[i] != '\0'; ++i)
        {
            exist = false;

            for (j = 0; j < ptr->child.size(); ++j) if (ptr->child[j]->data == name[i])
            {
                ptr = ptr->child[j];

                exist = true;
                break;
            }

            if (exist == false) return nullptr;
        }

        name = nullptr;
        return ptr->card;
    }

    void autoComplete(char* prefix)
    {
        prepare();
        if(head->dataRepeated>0) autoComplete2(prefix);
        else printf("\nThe Tree Is Empty Now !!!\n");

        prefix=nullptr;
    }

    void intersections(char* prefix)
    {
        prepare();
        if(head->dataRepeated>0) intersections2(prefix);
        else printf("\nThe Tree Is Empty Now !!!\n");

        prefix=nullptr;
    }

    void printLex()
    {
        if(head->dataRepeated>0) printLex2(head);
        else printf("\nThe Tree Is Empty Now !!!\n");
    }

    void printLev()
    {
        level=0;
        if(head->dataRepeated>0) {q1.push(head); printLev2();}
        else printf("\nThe Tree Is Empty Now !!!\n");
    }

    int size()
    {
        return head->dataRepeated;
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
    NodeTrie* head, * ptr, * ptr2;
    int i, j;
    int start, end, mid;
    bool exist, exist2, master;
    int insertedCount, deletedCount;

    personalCard* card;
    char* name;

    queue<NodeTrie*> q1,q2;
    int level;



    void prepare()
    {
        ptr = ptr2 = head;
        i = j = start = end = mid = exist = exist2 = 0;
    }

    void insertByName2(NodeTrie* node)
    {
        exist2 = 0;
        for (int i = 0; i < node->child.size(); ++i) if (name[j] == node->child[i]->data)
        {
            ++j;
            ptr2 = node->child[i];
            exist2 = 1;
            break;
        }

        if (!exist2)
        {
            ptr = node;
            ptr2 = new NodeTrie(name[j],node->level,node->prefix);
            insertWithSorting(); // useful when we want print it in lexicographical order , thanks Trie algorithm
            ++j;
        }

        if (name[j] != '\0') insertByName2(ptr2);
        else if (ptr2->card == nullptr) ptr2->card = card, ptr2->dataRepeated++, exist = 0;

        if (!exist) node->dataRepeated++;

        ptr=node;
        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    bool insertWithSorting()
    {
        start = 0;
        end = (ptr->child.size()) - 1;
        mid = (start + end) / 2;

        if (end < 0 || (ptr2->data > ptr->child[end]->data))
        {
            ptr->child.push_back(ptr2);
            mid = end + 1;
            return 0;
        }

        while (true)
        {
            if (ptr2->data < ptr->child[mid]->data && (mid == 0 || ptr2->data > ptr->child[mid - 1]->data))
            {
                ptr->child.insert(ptr->child.begin() + mid, ptr2);
                return 0;
            }

            else if (ptr2->data < ptr->child[mid]->data) end = mid - 1, mid = (start + end) / 2;

            else if (ptr2->data > ptr->child[mid]->data) start = mid + 1, mid = (start + end) / 2;
        }
    }

    void deleteByName2(NodeTrie* node)
    {
        for (int i = 0; i < node->child.size(); ++i) if (name[j] == node->child[i]->data)
        {
            ++j;

            if (name[j] != '\0') deleteByName2(node->child[i]);

            else if (node->child[i]->card != nullptr)
            {
                exist = 1;

                if(master) delete node->child[i]->card;

                node->child[i]->card = nullptr;
            }

            if (exist)
            {
                node->child[i]->dataRepeated--;

                if(node->child[i]->dataRepeated==0)
                {
                    if(master) delete node->child[i];

                    node->child[i] = nullptr;
                    node->child.erase(node->child.begin() + i);
                }
            }

            break;
        }

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void autoComplete2(char* prefix)
    {
        prepare();
        exist = true;
        for (i = 0; prefix[i] != '\0' && exist; ++i)
        {
            exist = false;

            for (j = 0; j < ptr->child.size(); ++j) if (ptr->child[j]->data == prefix[i])
                {
                    ptr = ptr->child[j];
                    exist = true;
                    break;
                }
        }

        if (exist)
        {
            printf("\n\n\n\n all actors start with ( %s ) :: \n", prefix);
            printLex2(ptr);
        }

        else  printf("\nNot Found This Prefix ( %s ) !!!\n",prefix);

        prefix = nullptr;
    }

    void intersections2(char* prefix)
    {
        prepare();
        exist = true;
        for (i = 0; prefix[i] != '\0' && exist; ++i)
        {
            exist = false;

            for (j = 0; j < ptr->child.size(); ++j) if (ptr->child[j]->data == prefix[i])
                {
                    ptr = ptr->child[j];

                    exist2=false;

                    if (ptr->card != nullptr)
                    {
                        printf("\n\n\n\nall actors intersection with first ( %i ) characters exactly :: \n", i + 1);
                        ptr->card->print();
                        exist2=true;
                    }

                    for (j = 0; j < ptr->child.size(); ++j) if (ptr->child[j]->data != prefix[i + 1])
                        {
                            if (!exist2) exist2=true, printf("\n\n\n\nall actors intersection with first ( %i ) characters exactly :: \n", i + 1);
                            printLex2(ptr->child[j]);
                        }

                    exist = true;
                    break;
                }
        }

        prefix = nullptr;
    }

    void printLex2(NodeTrie* node)
    {
        if (node->card != nullptr) node->card->print();

        for (int i = 0; i < node->child.size(); ++i) printLex2(node->child[i]);

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void printLev2()
    {
        while(q1.size()) // shipping the level
        {
            q2.push(q1.front());

            q1.pop();
        }


        printf("Level[ %i ]: ",level++);

        while(q2.size()) // print the level , then shipping next level
        {
            if(q2.front()->card!=nullptr) printf("[%s] , ",q2.front()->prefix);
            else printf("%s , ",q2.front()->prefix);

            for(int i=0;i<q2.front()->child.size();++i) q2.front()->data, q1.push(q2.front()->child[i]);

            q2.pop();
        }

        printf("\n\n\n");

        if(q1.size()) printLev2();
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

void interestedPerson(Trie* tree,bool flag)
{
    char* name=new char[100];

    char useless; // useless: using to handle last ( \n ) from user before read next string
    int i=0, *option=new int;

    for(i=0;i<100;++i) name[i]='\0';

    printf("----------------------------------------------\n");
    while(true)
    {
        printf("\nInterested Name: ");
        scanf("%c",&useless);
        readChar(name);

        if(tree->searchByName(name)!=nullptr)
        {
            if(flag) {tree->deleteByName(name);          printf("Successful Remove Person Process  (-_-)\n\n\n");}
            else     {tree->searchByName(name)->print(); printf("Successful Search Person Process  (-_-)\n\n\n");}
            break;
        }

        else
        {
            printf("----------------------------------------------\n");
            printf("this name not exist !!!\n\n"
                    "[ 0 ] for return previous step\n"
                    "[ 1 ] for try again by another name\n\n"
                    "[ 2 ] for auto complete the entered name\n"
                    "[ 3 ] for intersection with entered name\n");
            printf("----------------------------------------------\n\n");

        }

        readInteger(option,0,3);


        if(*option==0) break;
        else if(*option==2) tree->autoComplete(name);
        else if(*option==3) tree->intersections(name);

        printf("\n\n\n");
    }
    printf("----------------------------------------------\n");


    delete[] name;
    delete option;
    name=nullptr;
    option=nullptr;

    tree=nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
}

void addPerson(Trie* tree)
{
    char* name=new char[100];
    char* address=new char[100];
    int* age=new int;
    int* rank=new int;

    char useless; // useless: using to handle last ( \n ) from user before read next string
    int i=0;

    for(i=0;i<100;++i) name[i]='\0', address[i]='\0';

    printf("----------------------------------------------\n");

    printf("\nName: "); // no allow duplicate name
    scanf("%c",&useless);
    while(true)
    {
        readChar(name);

        if(!tree->searchByName(name)) break;
        else printf("\nthis name is duplicated with exist name , please enter another Name :: ");
    }

    printf("\nAddress: "); // allow duplicate address
    readChar(address);

    printf("\nAge: "); // allow duplicate age
    readInteger(age,minAge,maxAge);

    printf("\nRank: "); // allow duplicate Rank
    readInteger(rank,minRank,maxRank);

    printf("----------------------------------------------\n");

    printf("Successful Add Person Process  (-_-)\n\n\n");


    personalCard* card=new personalCard(name,address,age,rank);

    tree->insertByName(card->name,card);

    name=address=nullptr;
    age=rank=nullptr;

    card=nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    tree=nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
}

void test(Trie* tree)
{
    int* option=new int;
    while(true)
    {
        printf("----------------------------------------------\n");
        printf("[ 0 ] for exit whole program\n\n"
                      "[ 1 ] for add new person\n"
                      "[ 2 ] for search exit person\n"
                      "[ 3 ] for remove exit person\n\n"
                      "[ 4 ] for print count members\n"
                      "[ 5 ] for print tree  members\n" // sorted by lexicographically order
                      "[ 6 ] for print tree status\n"); // sorted by lexicographically order , but print before this tree levels ( how stored data in the tree )
        printf("----------------------------------------------\n");
        printf("select :: ");

        readInteger(option,0,6);

        if(*option==0) break;
        else if(*option==1) addPerson(tree);
        else if(*option==2) interestedPerson(tree,0);
        else if(*option==3) interestedPerson(tree,1);
        else if(*option==4) printf("\nTree Size: %i\n\n",tree->size());
        else
        {
            printf("\n");
            if(*option==6) tree->printLev();
            printf("\n");

            tree->printLex();
            printf("\n\n\n");
        }
    }

    printf("\n\n\t\tSuccessful ( SHUT DOWN )\n\n");

    delete option;
    option=nullptr;

    tree=nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// main()
int main()
{
    Trie tree(1);
    test(&tree);

    // this Trie sorting data by ( Name )
    // Creation condition ( Unique Name )

    return 0;
}
