//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// used libraries
#pragma warning(disable : 4996) // must using if run program in visual studio , Not important for other IDEs

#include<cstdio>
#include<cmath>
#include<ctime>
#include<thread>
#include<vector>
#include<queue>

using namespace std;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// #defines  &  global variables
#define maxChar 100
#define maxValueCount 10000
#define maxAvailableSecond 100

int maxThreads;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// lvl0 Abstract Base Class ( ABC )
// ( Dispatcher Helper Class )
class lvl0_Drug
{
public:
    // prefer initialize ( pointers ) before constructing object for maximize safety
    lvl0_Drug(char* name, char* address, int *value) : name(name), address(address), value(value), count(0), copied(0)
    {
        publicID=ID_Drug++;

        name = address = nullptr;
        value=nullptr;
    }

    virtual ~lvl0_Drug() // virtual destructor because we use polymorphism
    {
        if(name!=nullptr)    delete[] name,    name=nullptr;
        if(address!=nullptr) delete[] address, address=nullptr;
        if(value!=nullptr)   delete value,     value=nullptr;
    }

    virtual lvl0_Drug* copy() = 0;        // pure simulating virtual copy constructor --> must exist override in all sub classes

    virtual void increase() = 0;          // pure virtual function --> must exist override in all sub classes

    virtual void decrease() = 0;          // pure virtual function --> must exist override in all sub classes

    virtual int getAuto() = 0;            // pure virtual function --> must exist override in all sub classes

    virtual int getAutomated() = 0;       // pure virtual function --> must exist override in all sub classes

    virtual int StopAuto() = 0;           // pure virtual function --> must exist override in all sub classes

    virtual void print(bool flag=1) = 0;  // pure virtual function --> must exist override in all sub classes


    char* name;
    char* address;

    int* value;

    int count;
    int copied;

    int publicID;
    int privateID;
    int type;

    static int ID_Drug; // static because count any object created or inherited from this class , protected because access it in sub classes
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// lvl1
// DefaultCount
class lvl1_OneByOne : public lvl0_Drug
{
public:
    // prefer initialize ( pointers ) before constructing object for maximize safety
    lvl1_OneByOne(char* name, char* address) : lvl0_Drug(name, address,new int(1))
    {
        privateID=ID_OneByOne++;
        type=1;

        name = address = nullptr;
    }

    // ~lvl1_OneByOne() {} // no need destructor because no own any pointer cause memory leak , base class destructor is enough

    lvl0_Drug* copy() override // simulating virtual copy constructor
    {
        // as a class contain pointers --must--> create new location for each pointer before copy it

        // if a class contain only static variables --can--> copy it by one line   -->   { return  new  AbstractBaseClass*  ( *this ); }

        char* name2=new char[maxChar];
        char* address2=new char[maxChar];
        for(int i=0;name[i]!='\0';++i) name2[i]=name[i], address2[i]=address[i];


        lvl0_Drug* ptr=new lvl1_OneByOne(name2,address2);
        --ID_Drug;
        --ID_OneByOne;

        ptr->publicID=publicID;
        ptr->privateID=privateID;

        ptr->count=count;
        ptr->copied=copied=1;

        return ptr;
    }

    void increase() override
    {
        ++count;
    }

    void decrease() override
    {
        --count;
    }

    virtual int getAuto()      { return 0; } // compiler not allowed consective pure virtual function --> this is simulating virtual function

    virtual int getAutomated() { return 0; } // compiler not allowed consective pure virtual function --> this is simulating virtual function

    virtual int StopAuto()     { return 0; } // compiler not allowed consective pure virtual function --> this is simulating virtual function

    void print(bool flag=1) override
    {
        if(flag) printf("\n=============================================================================================\n");
        printf("Name    :: %s\n", name);
        printf("Address :: %s\n", address);
        printf("Public  ID :: %i\n", publicID);
        printf("Private ID :: %i\n", privateID);
        printf("Type   :: %i\n",type);
        printf("Copied :: %i\n\n",copied);
        printf("Count  :: %i\n\n", count);
        printf("Can Increase & Decrease Whenever Call By ( 1 )\n");
        printf("=============================================================================================\n");

    }


    static int ID_OneByOne; // static because count any object created from this class , private because no sub classes want access it
};

// CountWithValue
class lvl1_ByQuantity : public lvl0_Drug
{
public:
    // prefer initialize ( pointers ) before constructing object for maximize safety
    lvl1_ByQuantity(char* name, char* address, int* value) : lvl0_Drug(name, address, value)
    {
        privateID=ID_ByQuantity++;
        type=2;

        name = address = nullptr;
        value=nullptr;
    }

    // ~lvl1_ByQuantity() {} // no need destructor because no own any pointer cause memory leak , base class destructor is enough

    lvl0_Drug* copy() override // simulating virtual copy constructor
    {
        // as a class contain pointers --must--> create new location for each pointer before copy it

        // if a class contain only static variables --can--> copy it by one line   -->   { return  new  AbstractBaseClass*  ( *this ); }

        char* name2=new char[maxChar];
        char* address2=new char[maxChar];
        for(int i=0;name[i]!='\0';++i) name2[i]=name[i], address2[i]=address[i];

        lvl0_Drug* ptr=new lvl1_ByQuantity(name2,address2,new int(*value));
        --ID_Drug;
        --ID_ByQuantity;

        ptr->publicID=publicID;
        ptr->privateID=privateID;

        ptr->count=count;
        ptr->copied=copied=1;

        return ptr;
    }

    void increase() override
    {
        count += *value;
    }

    void decrease() override
    {
        count -= *value;
    }

    virtual int getAuto()      { return 0; } // compiler not allowed consective pure virtual function --> this is simulating virtual function

    virtual int getAutomated() { return 0; } // compiler not allowed consective pure virtual function --> this is simulating virtual function

    virtual int StopAuto()     { return 0; } // compiler not allowed consective pure virtual function --> this is simulating virtual function

    void print(bool flag=1) override
    {
        if(flag) printf("\n=============================================================================================\n");
        printf("Name     :: %s\n", name);
        printf("Address  :: %s\n", address);
        printf("Public  ID :: %i\n", publicID);
        printf("Private ID :: %i\n", privateID);
        printf("Type   :: %i\n",type);
        printf("Copied :: %i\n\n", copied);
        printf("Count  :: %i\n\n", count);
        printf("Can Increase & Decrease Whenever Call By ( %i )\n",*value);
        printf("=============================================================================================\n");
    }


    static int ID_ByQuantity; // static because count any object created or inherited from this class , protected because access it in sub classes
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// lvl2
// AutoIncrease
class lvl2_BuyingByQuantity final : public lvl1_ByQuantity
{
public:
    // prefer initialize ( pointers ) before constructing object for maximize safety
    lvl2_BuyingByQuantity(char* name, char* address, int* value, int* autoIncrease) : lvl1_ByQuantity(name, address, value) ,autoIncrease(autoIncrease)
    {
        // because object from this class increase the Parent Class ID ( lvl1_ByQuantity :: ID_ByQuantity ) Unnecessarily , because of the inheritance relationship call ( lvl1_ByQuantity :: Constructor )
        --ID_ByQuantity;

        privateID=ID_BuyingByQuantity++;
        type=3;
        running=0;
        automated=0;

        name = address = nullptr;
        value=nullptr;
    }

    ~lvl2_BuyingByQuantity()
    {
        if(autoIncrease!=nullptr) delete autoIncrease, autoIncrease=nullptr;

        running = 0;
    }

    lvl0_Drug* copy() override // simulating virtual copy constructor
    {
        // as a class contain pointers --must--> create new location for each pointer before copy it

        // if a class contain only static variables --can--> copy it by one line   -->   { return  new  AbstractBaseClass*  ( *this ); }

        char* name2=new char[maxChar];
        char* address2=new char[maxChar];
        for(int i=0;name[i]!='\0';++i) name2[i]=name[i], address2[i]=address[i];

        lvl0_Drug* ptr=new lvl2_BuyingByQuantity(name2,address2,new int(*value),new int(*autoIncrease));
        --ID_Drug;
        --ID_BuyingByQuantity;

        ptr->publicID=publicID;
        ptr->privateID=privateID;

        ptr->count=count;
        ptr->copied=copied=1;

        return ptr;
    }

    void increase() override // automatic increasing by ( value ) --> each ( autoIncrease ) second
    {
        automated=0;
        running = 1;

        double clk, clk2=clock()/1000.0 + *autoIncrease; // translate time from a ( milli second ) to ( second )
        while(running)
        {
            clk=clock()/1000.0;

            if (clk >= clk2)
            {
                count += *value;

                ++automated;

                clk2 = clock() / 1000.0 + *autoIncrease;
            }
        }
    }

    // void decrease() override {--- --- ---} no override it because that's same in my parent class --> lvl1_ByQuantity::decrease()

    int getAuto()
    {
        return *autoIncrease;
    }

    int getAutomated()
    {
        return automated;
    }

    int StopAuto() override
    {
        running = 0;
        return count;
    }

    void print(bool flag=1) override
    {
        if(flag) printf("\n=============================================================================================\n");
        printf("Name    :: %s\n", name);
        printf("Address :: %s\n", address);
        printf("Public  ID :: %i\n", publicID);
        printf("Private ID :: %i\n", privateID);
        printf("Type   :: %i\n",type);
        printf("Copied :: %i\n\n", copied);
        printf("Count  :: %i\n\n", count);
        printf("Can Decrease Whenever Call By ( %i ) , Can AutoIncrease By ( %i ) Each ( %i ) Seconds\n",*value,*value,*autoIncrease);
        printf("=============================================================================================\n");
    }


    static int ID_BuyingByQuantity; // static because count any object created from this class , private because no sub classes want access it
    int automated;

private: // no protected but private because this is final class ( no own sub classes )
    int* autoIncrease;
    bool running;
};

// AutoDecrease
class lvl2_SellingByQuantity final : public lvl1_ByQuantity
{
public:
    // prefer initialize ( pointers ) before constructing object for maximize safety
    lvl2_SellingByQuantity(char* name, char* address, int* value, int* autoDecrease) : lvl1_ByQuantity(name, address, value) , autoDecrease(autoDecrease)
    {
        // because object from this class increase the Parent Class ID ( lvl1_ByQuantity :: ID_ByQuantity ) Unnecessarily , because of the inheritance relationship call ( lvl1_ByQuantity :: Constructor )
        --ID_ByQuantity;

        privateID=ID_SellingByQuantity++;
        type=4;
        running=0;
        automated=0;

        name = address = nullptr;
        value=nullptr;
    }

    ~lvl2_SellingByQuantity()
    {
        if(autoDecrease!=nullptr) delete autoDecrease, autoDecrease=nullptr;

        running = 0;
    }

    lvl0_Drug* copy() override // simulating virtual copy constructor
    {
        // as a class contain pointers --must--> create new location for each pointer before copy it

        // if a class contain only static variables --can--> copy it by one line   -->   { return  new  AbstractBaseClass*  ( *this ); }

        char* name2=new char[maxChar];
        char* address2=new char[maxChar];
        for(int i=0;name[i]!='\0';++i) name2[i]=name[i], address2[i]=address[i];

        lvl0_Drug* ptr=new lvl2_SellingByQuantity(name2,address2,new int(*value),new int(*autoDecrease));
        --ID_Drug;
        --ID_SellingByQuantity;

        ptr->publicID=publicID;
        ptr->privateID=privateID;

        ptr->count=count;
        ptr->copied=copied=1;

        return ptr;
    }

    // void increase() override {--- --- ---} no override it because that's same in my parent class --> lvl1_ByQuantity::increase()

    void decrease() override // automatic decreasing by ( value ) --> each ( autoDecrease ) second
    {
        automated=0;
        running = 1;

        double clk, clk2=clock()/1000.0 + *autoDecrease; // clock() / 1000   -->   translate time from a ( milli second ) to ( second )
        while(running)
        {
            clk=clock()/1000.0;

            if (clk >= clk2)
            {
                count -= *value;

                ++automated;

                clk2 = clock() / 1000.0 + *autoDecrease;
            }
        }
    }

    int getAuto()
    {
        return *autoDecrease;
    }

    int getAutomated()
    {
        return automated;
    }

    int StopAuto() override
    {
        running = 0;
        return count;
    }

    void print(bool flag=1) override
    {
        if(flag) printf("\n=============================================================================================\n");
        printf("Name    :: %s\n", name);
        printf("Address :: %s\n", address);
        printf("Public  ID :: %i\n", publicID);
        printf("Private ID :: %i\n", privateID);
        printf("Type   :: %i\n",type);
        printf("Copied :: %i\n\n", copied);
        printf("Count  :: %i\n\n", count);
        printf("Can Increase Whenever Call By ( %i ) , Can AutoDecrease By ( %i ) Each ( %i ) Seconds\n",*value,*value,*autoDecrease);
        printf("=============================================================================================\n");

    }


    static int ID_SellingByQuantity; // static because count any object created from this class , private because no sub classes want access it
    int automated;

private: // no protected but private because this is final class ( no own sub classes )
    int* autoDecrease;
    bool running;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// static variables
int lvl0_Drug::ID_Drug = 1;
int lvl1_OneByOne::ID_OneByOne = 1;
int lvl1_ByQuantity::ID_ByQuantity = 1;
int lvl2_BuyingByQuantity::ID_BuyingByQuantity = 1;
int lvl2_SellingByQuantity::ID_SellingByQuantity = 1;



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

    char data;                // one character represent node
    char* prefix;             // small string represent path from root to me
    int level;                // length from root to me
    int dataRepeated;         // how many words passed by me
    vector<NodeTrie*> child;  // contain all children nodes
    lvl0_Drug* card;          // if me is a last character in a word --> card of this word , else --> null
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

    bool insertByName(char* name, lvl0_Drug* card)
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

    lvl0_Drug* searchByName(char* name)
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

        name = nullptr;  // must assignment ( null ) because no destroy the original location at the end of function
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

    lvl0_Drug* card;
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



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Balanced Binary Search Tree class
struct NodeBBST
{
    // prefer initialize pointer before constructing object for maximize safety
    NodeBBST(const int& value, lvl0_Drug* card) : card(card) , left(nullptr) , right(nullptr)
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
        if(left!=nullptr)  delete left,  left=nullptr;
        if(right!=nullptr) delete right, right=nullptr;
        if(card!=nullptr)  delete card,  card=nullptr;
    }


    int value, smallest, largest; // value ( value used for sorting tree ) , smallest ( smallest value in my sub tree ) , largest ( largest value in my sub tree )
    int length, size;             // length ( longest path in my sub tree ) , size ( size of my sub tree )

    lvl0_Drug* card;              // card ( this is complete card of my data )
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
        ptr=ptr2=nullptr;
        if(head!=nullptr) delete head, head=nullptr;
    }

    bool insertByValue(const int& value, lvl0_Drug* card)
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
        card = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
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

    lvl0_Drug* searchByValue(const int& value)
    {
        prepare();
        tmp = value;
        if(head!=nullptr) return searchByValue2();
        return nullptr;
    }

    void copiedKilled(const int& publicID)
    {
        prepare();
        ptr->card = searchByValue(publicID);

        if (ptr->card != nullptr) ptr->card->copied = 0;
    }



    void printTreeShape()
    {
        prepare();
        if(head!=nullptr)
        {
            level=0;

            q1.push(head);
            //q2 --> empty

            printTreeShape2();
            printByLevel(); // can call this function outside   printTreeShape();
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

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
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

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return ptr;
    }

    lvl0_Drug* searchByValue2()
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

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return ptr;
    }

    NodeBBST* smallest(NodeBBST* node)
    {
        if (node->left != nullptr) node->left = smallest(node->left);

        else ptr2 = node, ptr = node->right, ptr2->right = nullptr, node = ptr;


        ptr = balanced(node);

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
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

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
        return ptr;
    }

    int len(NodeBBST* node)
    {
        if (node == nullptr) tmp = 0;
        else                 tmp=node->length;

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
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

        node=nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
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

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void printPostfixes2(NodeBBST* node)
    {
        if (node->left != nullptr)  printPostfixes2(node->left);
        if (node->right != nullptr) printPostfixes2(node->right);
        node->card->print();
        printf("( Node SubTree Length: %i )\n",node->length);

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void printAscending2(NodeBBST* node)
    {
        if (node->left != nullptr)  printAscending2(node->left);
        node->card->print();
        if (node->right != nullptr) printAscending2(node->right);

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void printDescending2(NodeBBST* node)
    {
        if (node->right != nullptr) printDescending2(node->right);
        node->card->print();
        if (node->left != nullptr)  printDescending2(node->left);

        node = nullptr;  // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

    void printRange2(NodeBBST* node)
    {
        if (node->left != nullptr && node->left->largest >= tmp)     printRange2(node->left);
        if (node->value >= tmp && node->value <= tmp2)               node->card->print();
        if (node->right != nullptr && node->right->smallest <= tmp2) printRange2(node->right);

        node = nullptr; // must assignment ( null ) because no call class :: destructor which destroy the original location at the end of function
    }

};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DrugSystem class ( target of system )
class DrugSystem
{
public:

    DrugSystem() : ptr(nullptr), ptr2(nullptr)
    {
        prepareNew();

        for(int i=0;i<40;++i) str4[0][i]=str4[1][i]=str4[2][i]=str4[3][i]='\0';
        assignChar("Search", "Copy", "Delete", "( Increase Or Decrease )");

        maxThreads = thread::hardware_concurrency() - 1; // my computer hardware maxThreads == 8 , so we can run 7 concurrency threads in the background
    }

    ~DrugSystem()
    {
        background(1);
        printf("\nSystem Destructor Terminate All Above Background Threads Before Exit The System For Save Saftey\n");



        // must manual clear for trees before call his destructor because we stored actual data at once and access by ( 4 Trees )

        // if destructor called by traditional way --> first tree called destructor success for destroyed actual data , and other tree when call his destructor give ERROR !!!

        // so we release all not master trees to ( NULL )   --then-->   delete actual data by master tree at the end

        // void clear()
        // {
        //     if(master) delete head;
        //     head=nullptr;
        // }


        for(i=0;i<5;++i) tree_ID[i].clear(), treeCopied_ID[i].clear();
        tree_address.clear();
        treeCopied_address.clear();
        tree_name.clear();        // master tree for original actors
        treeCopied_name.clear();  // master tree for copied actors



        if(ptr!=nullptr)  delete ptr;
        if(ptr2!=nullptr) delete ptr2;

        if(str!=nullptr) delete[] str;
        if(str2!=nullptr) delete[] str2;

        if(choseActor!=nullptr) delete choseActor;
        if(countActor!=nullptr) delete countActor;

        if(tmp!=nullptr)  delete tmp;
        if(tmp2!=nullptr) delete tmp2;
        if(tmp3!=nullptr) delete tmp3;
        if(tmp4!=nullptr) delete tmp4;

        ptr=ptr2=nullptr;
        str=str2=nullptr;
        choseActor=countActor=tmp=tmp2=tmp3=tmp4=nullptr;
        threadRun= nullptr;

        printf("\nEnd System Destructor\n");
    }



    bool chatbot()
    {
        prepare();

        printf("\n---------------------------------------------------------------------------------------------\n");
        printf("\t\t\t\t( SYSTEM HOME PAGE )\n\t\t\t( allow doing below operations )\n\n");
        printf("---------------------------------------------------------------------------------------------\n");
        printf("[ 0 ] Nothing any operation --> ( SYSTEM EXIT ) \n"
                       "[ 1 ] Create Actor\n"
                       "[ 2 ] Search Actor\n"
                       "[ 3 ] Copy Actor\n"
                       "[ 4 ] Delete Actor\n"
                       "[ 5 ] ( Increase Or Decrease ) Operation\n"
                       "[ 6 ] Print All Actors\n"
                       "[ 7 ] show ( BACKGROUND PAGE ) --> ( showing operations running in the background )\n");
        printf("---------------------------------------------------------------------------------------------\n\n");

        readInteger(tmp4,0,7);

        if(*tmp4==0)                  { printf("\n\n\t\tSuccessful ( SYSTEM SHUT DOWN )\n\n"); return 0; }
        else if(*tmp4==1)             create();
        else if(*tmp4>=2 && *tmp4<=5) search();
        else if(*tmp4==6)             printAll();
        else if(*tmp4==7)             background();

        chatbot();
        return 0;
    }



private:
    // data stored only once in memory
    // but we can access it in different ways through the different trees that no stored actual data but refer to this data by pointer , each tree according to its task
    // this pointer ( 4 byte --> OS32 ) or ( 8 byte --> OS64 ) regardless actual size of data type who referring to  -->  thanks pointer for save time and memory

    Trie tree_name = new Trie(1); // store actors CREATED sorted by ( name ) using Trie algorithm for maximize performance , ( master == 1 ) --> give permission to delete actual data in memory
    Trie treeCopied_name = new Trie;      // store actors COPIED  sorted by ( name ) using Trie algorithm for maximize performance

    Trie tree_address;                    // store actors CREATED sorted by ( address ) using Trie algorithm for maximize performance
    Trie treeCopied_address;              // store actors COPIED  sorted by ( address ) using Trie algorithm for maximize performance

    BBST tree_ID[5];                      // store actors CREATED in public tree[ 0 ]  &  in private tree [ 1 : 4 ]
    BBST treeCopied_ID[5];                // store actors COPIED  in public tree[ 0 ]  &  in private tree [ 1 : 4 ]



    int *choseActor, *countActor;     // using during ( chose actor type you want create , determine count of actor you want create )
    int i, j;                         // created once and using it multiple times in loops , for saving creating time

    lvl0_Drug *ptr, *ptr2;            // using in temporary assignment  before insert it in ( actor ) or ( actorCopied )
    int *tmp, *tmp2, *tmp3, *tmp4;    // using in temporary some of calculation
    char *str, *str2;                 // using in temporary assignment of actor ( name , address ) , pure ( char ) is faster than string
    char str4[4][40],useless;         // str4: using for show user type of operation doing now during search , useless: using to handle last ( \n ) from user before read next string

    thread* threadRun;                // using in temporary assignment new thread
    vector<thread*> threadRunning;    // contain all threads active running in the background
    vector<lvl0_Drug*> threadPointer; // give ability to access the actor which have thread in active running in the background



    void prepare()
    {
        ptr=ptr2=nullptr;
        *choseActor=*countActor=i=j=*tmp=*tmp2=*tmp3=*tmp4=0;
    }

    void prepareNew()
    {
        ptr=ptr2=nullptr;

        str = new char[maxChar+1];
        str2 = new char[maxChar+1];
        for (int i = 0; i < maxChar+1; ++i) str[i] = str2[i] = '\0';

        choseActor=new int(0);
        countActor=new int(0);

        tmp=new int(0);
        tmp2=new int(0);
        tmp3=new int(0);
        tmp4=new int(0);

        i = j = 0;
    }

    void readInteger(int* num, int a,int b)
    {
        int ok=1;
        char* check=new char[100];

        printf("select in range ( %i : %i ): ",a,b);
        while(true)
        {
            *num=0; ok=1;

            scanf("%s",check);

            for(int i=0; check[i]!='\0' ;++i) if(check[i]<'0' || check[i]>'9') {ok=0; break;}

            if(ok) for(int i=0; check[i]!='\0' ;++i) *num=(*num*10) + (check[i]-'0');
            else
            {
                printf("\nplease enter only numbers !!!\n\nselect in range ( %i : %i ): ",a,b);
                continue;
            }

            if(*num>=a && *num<=b) break;
            else printf("\nplease select in  range  ( %i : %i ): ",a,b);
        }

        delete[] check;
        check=nullptr;
    }

    void readChar(char* STR)
    {
        // continue reading any character until user push ENTER --> ( \n )

        for (j = 0;j==0 || STR[j - 1] != '\n'; ++j) scanf("%c", &STR[j]);
        STR[j-1]='\0';

        for(j=0;STR[j]!='\0';++j) if(STR[j]>='A' && STR[j]<='Z') STR[j]+=('a'-'A');

        STR=nullptr;
    }

    void cleanChar(char* STR)
    {
        for (j = 0; STR[j] != '\0'; ++j) STR[j] = '\0';
    }

    void assignChar(const char* f0, const char* f1, const char* f2, const char* f3)
    {
        for (i = 0; i == 0 || f0[i - 1] != '\0'; ++i) str4[0][i] = f0[i];
        for (i = 0; i == 0 || f1[i - 1] != '\0'; ++i) str4[1][i] = f1[i];
        for (i = 0; i == 0 || f2[i - 1] != '\0'; ++i) str4[2][i] = f2[i];
        for (i = 0; i == 0 || f3[i - 1] != '\0'; ++i) str4[3][i] = f3[i];
    }

    void actorDetails(const int num)
    {
        if (num == 1)
        {
            printf("\n---------------------------------------------------------------------------------------------\n");
            printf("Actor [ 1 ]: (  Name  ,  Address  ,  Public ID  ,  Private ID  ,  Count  ) \n\n");
            printf("Increase() :: can increase his actor COUNT by ( 1 ) , Whenever Call\n");
            printf("Decrease() :: can decrease his actor COUNT by ( 1 ) , Whenever Call\n");
            printf("---------------------------------------------------------------------------------------------\n");
        }

        else if (num == 2)
        {
            printf("\n---------------------------------------------------------------------------------------------\n");
            printf("Actor [ 2 ]: (  Name  ,  Address  ,  Public ID  ,  Private ID  ,  Count  ) \n\n");
            printf("Increase() :: can increase his actor COUNT by ( X :: determine by you ) , Whenever Call\n");
            printf("Decrease() :: can decrease his actor COUNT by ( X :: determine by you ) , Whenever Call\n");
            printf("---------------------------------------------------------------------------------------------\n");
        }

        else if (num == 3)
        {
            printf("\n---------------------------------------------------------------------------------------------\n");
            printf("Actor [ 3 ]: (  Name  ,  Address  ,  Public ID  ,  Private ID  ,  Count  ,  AutoIncrease  ) \n\n");
            printf("Increase() :: can increase his actor COUNT by ( X :: determine by you ) , every ( Y :: determine by you ) second\n");
            printf("Decrease() :: can decrease his actor COUNT by ( X :: determine by you ) , Whenever Call\n");
            printf("---------------------------------------------------------------------------------------------\n");
        }

        else if (num == 4)
        {
            printf("\n---------------------------------------------------------------------------------------------\n");
            printf("Actor [ 4 ]: (  Name  ,  Address  ,  Public ID  ,  Private ID  ,  Count  ,  AutoDecrease  ) \n\n");
            printf("Increase() :: can increase his actor COUNT by ( X :: determine by you ) , Whenever Call\n");
            printf("Decrease() :: can decrease his actor COUNT by ( X :: determine by yoy ) , every ( Y :: determine by you ) second\n");
            printf("---------------------------------------------------------------------------------------------\n");
        }
    }



    void create()
    {
        actorDetails(1);
        actorDetails(2);
        actorDetails(3);
        actorDetails(4);

        while(true)
        {
            printf("\n---------------------------------------------------------------------------------------------\n");

            printf("\n[ 0 ] for return ( HOME PAGE )\n"
                             "[ 1 ] for create new actor\n\n");

            readInteger(countActor,0,1);

            if(*countActor==1)
            {
                printf("\nselect Interested Actor type [ 1 , 2 , 3 , 4 ]\n\n");

                readInteger(choseActor,1,4);

                create2();
            }

            else break;
        }
    }

    void create2()
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        if (*choseActor == 1)
        {
            printf("\nMust Enter Selected Actor Features:\n");
            actorDetails(1);

            getName();
            getAddress();

            ptr = new lvl1_OneByOne(str, str2); // create ( lvl1_OneByOne ) from ( lvl0_Drug ) , and can handle all function easy , thanks inheritance and polymorphism ( virtual pointer )
        }

        else if (*choseActor == 2)
        {
            printf("\nMust Enter Selected Actor Features:\n");
            actorDetails(2);

            getName();
            getAddress();
            getValue();

            ptr = new lvl1_ByQuantity(str, str2, tmp);  // create ( lvl1_ByQuantity ) from ( lvl0_Drug ) , and can handle all function easy , thanks inheritance and polymorphism  ( virtual pointer )
        }

        else if (*choseActor == 3)
        {
            printf("\nMust Enter Selected Actor Features:\n");
            actorDetails(3);

            getName();
            getAddress();
            getValue();
            getClock(1);

            ptr = new lvl2_BuyingByQuantity(str, str2, tmp, tmp2); // create ( lvl2_BuyingByQuantity ) from ( lvl0_Drug ) , and can handle all function easy , thanks inheritance and polymorphism  ( virtual pointer )
        }

        else if (*choseActor == 4)
        {
            printf("\nMust Enter Selected Actor Features:\n");
            actorDetails(4);

            getName();
            getAddress();
            getValue();
            getClock(0);

            ptr = new lvl2_SellingByQuantity(str, str2, tmp, tmp2); // create ( lvl2_SellingByQuantity ) from ( lvl0_Drug ) , and can handle all function easy , thanks inheritance and polymorphism  ( virtual pointer )
        }

        tree_name.insertByName(ptr->name, ptr);//   ( master tree )   // insert this actor in ( Trie  :: tree_name )    contain all actors type , data sorted by ( name )
        tree_address.insertByName(ptr->address, ptr);                 // insert this actor in ( Trie  :: tree_address ) contain all actors type , data sorted by ( address )

        tree_ID[0].insertByValue(ptr->publicID, ptr);                 // insert this actor in ( BBST Tree  :: tree[ 0 ] )     contain all actors type , data sorted by ( public ID )
        tree_ID[*choseActor].insertByValue(ptr->privateID, ptr);      // insert this actor in ( BBST Tree  :: tree[ 1 : 4 ] ) contain once actor type , data sorted by ( private ID )

        prepareNew();

        printf("\n---------------------------------------------------------------------------------------------\n");

        printf("\nSuccessful Created New Actor Process (-_-)\n");
    }

    void getName()
    {
        printf("\nName: ");

        scanf("%c",&useless);

        while (true)
        {
            readChar(str);
            if (!tree_name.searchByName(str)) break;
            else cleanChar(str), printf("\nthis name already exist , please enter another Name: ");
        }
    }

    void getAddress()
    {
        printf("\nAddress: ");

        while (true)
        {
            readChar(str2);
            if (!tree_address.searchByName(str2)) break;
            else cleanChar(str2), printf("\nthis address already exist , please enter another Address: ");
        }
    }

    void getValue()
    {
        printf("\nValue ( X ): ");

        readInteger(tmp,1,maxValueCount);
    }

    void getClock(const bool flag)
    {
        if(flag) printf("\nAutoIncrease ( Y ): ");
        else     printf("\nAutoDecrease ( Y ): ");

        readInteger(tmp2,1,maxAvailableSecond);
    }



    void search()
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        *tmp = 1;
        printf("\n[ 0 ] for return ( HOME PAGE )\n"
                       "[ 1 ] for %s on ( Original Actors )\n", str4[*tmp4-2]);

        if (*tmp4 != 3) { printf("[ 2 ] for %s on ( Copied Actors )\n", str4[*tmp4-2]); ++*tmp; }
        printf("\n");

        readInteger(tmp3,0,*tmp);

        if(*tmp3==1 && tree_ID->size()==0)       printf("\nOriginal Tree Is Empty Now !!!\n");

        else if(*tmp3==2 && treeCopied_ID->size()==0) printf("\nCopied Tree Is Empty Now !!!\n");

        else if (*tmp3 > 0)
        {
            printf("\n[ 0 ] for return ( HOME PAGE )\n"
                           "[ 1 ] for find actor if you want %s it by ( Name )\n"
                           "[ 2 ] for find actor if you want %s it by ( Address )\n"
                           "[ 3 ] for find actor if you want %s it by ( Public  ID )\n"
                           "[ 4 ] for find actor if you want %s it by ( Private ID )\n\n", str4[*tmp4-2], str4[*tmp4-2], str4[*tmp4-2], str4[*tmp4-2]);

            readInteger(tmp,0,4);

            if (*tmp == 1)      searchByName();
            else if (*tmp == 2) searchByAddress();
            else if (*tmp == 3) searchByPublicID();
            else if (*tmp == 4) searchByPrivateID();
        }
    }

    void searchByName()
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        printf("\nInterested Name: ");

        cleanChar(str);

        scanf("%c",&useless);
        readChar(str);

        if(*tmp3 == 1)      ptr = tree_name.searchByName(str);
        else if(*tmp3 == 2) ptr = treeCopied_name.searchByName(str);

        if (ptr != nullptr)
        {
            ptr->print();
            options(1);

            cleanChar(str);
        }

        else
        {
            printf("\nthis name is not exist , may be you want help (-_-)\n\n");
            printf("[ 0 ] for return ( HOME PAGE )\n"
                           "[ 1 ] for try again\n"
                           "[ 2 ] for auto complete\n"
                           "[ 3 ] for all prefixes intersection\n\n");

            readInteger(tmp,0,3);

            if (*tmp == 0)      chatbot();
            else if (*tmp == 1) searchByName();

            else if (*tmp == 2 && *tmp3 == 1) tree_name.autoComplete(str);
            else if (*tmp == 2 && *tmp3 == 2) treeCopied_name.autoComplete(str);

            else if (*tmp == 3 && *tmp3 == 1) tree_name.intersections(str);
            else if (*tmp == 3 && *tmp3 == 2) treeCopied_name.intersections(str);

            cleanChar(str);
        }
    }

    void searchByAddress()
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        printf("\nInterested Address: ");

        cleanChar(str);

        scanf("%c",&useless);
        readChar(str);

        if(*tmp3 == 1)      ptr = tree_address.searchByName(str);
        else if(*tmp3 == 2) ptr = treeCopied_address.searchByName(str);

        if (ptr != nullptr)
        {
            ptr->print();
            options(2);

            cleanChar(str);
        }

        else
        {
            printf("\nthis name is not exist , may be you want help (-_-)\n\n");
            printf("[ 0 ] for return ( HOME PAGE )\n"
                           "[ 1 ] for try again\n"
                           "[ 2 ] for auto complete\n"
                           "[ 3 ] for all prefixes intersection\n\n");

            readInteger(tmp,0,3);

            if (*tmp == 0)      chatbot();
            else if (*tmp == 1) searchByAddress();

            else if (*tmp == 2 && *tmp3 == 1) tree_address.autoComplete(str);
            else if (*tmp == 2 && *tmp3 == 2) treeCopied_address.autoComplete(str);

            else if (*tmp == 3 && *tmp3 == 1) tree_address.intersections(str);
            else if (*tmp == 3 && *tmp3 == 2) treeCopied_address.intersections(str);

            cleanChar(str);
        }
    }

    void searchByPublicID()
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        printf("\nInterested ( Public ID ):");
        readInteger(tmp,1,lvl0_Drug::ID_Drug-1);



        if(*tmp3 == 1)      ptr = tree_ID[0].searchByValue(*tmp);
        else if(*tmp3 == 2) ptr = treeCopied_ID[0].searchByValue(*tmp);

        if (ptr != nullptr) ptr->print(), options(3);
        else
        {
            printf("\nThis Actor not found !!! , may be deleted from before\n\n"
                           "[ 0 ] for return ( HOME PAGE )\n"
                           "[ 1 ] for try again\n\n");

            readInteger(tmp,0,1);
            if(*tmp==1) searchByPublicID();
        }
    }

    void searchByPrivateID()
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        printf("\nInterested ( Actor Type ): ");
        readInteger(choseActor,1,4);

        if( (*tmp3 == 1 && tree_ID[*choseActor].size() > 0) || (*tmp3 == 2 && treeCopied_ID[*choseActor].size()>0) )
        {
            printf("\nInterested ( Actor Type[ %i ]: Private ID ): ",*choseActor);

            if(*choseActor==1)      readInteger(tmp,1,lvl1_OneByOne::ID_OneByOne-1);
            else if(*choseActor==2) readInteger(tmp,1,lvl1_ByQuantity::ID_ByQuantity-1);
            else if(*choseActor==3) readInteger(tmp,1,lvl2_BuyingByQuantity::ID_BuyingByQuantity-1);
            else if(*choseActor==4) readInteger(tmp,1,lvl2_SellingByQuantity::ID_SellingByQuantity-1);



            if(*tmp3 == 1)      ptr = tree_ID[*choseActor].searchByValue(*tmp);
            else if(*tmp3 == 2) ptr = treeCopied_ID[*choseActor].searchByValue(*tmp);

            if (ptr != nullptr) ptr->print(), options(4);
            else
            {
                printf("\nThis Actor not found !!! , may be deleted from before\n\n"
                               "[ 0 ] for return ( HOME PAGE )\n"
                               "[ 1 ] for try again\n\n");

                readInteger(tmp,0,1);
                if(*tmp==1) searchByPrivateID();
            }
        }

        else if(*tmp3 == 1) printf("\nType[ %i ] In Original Tree Is Empty Now !!!\n",*choseActor);
        else if(*tmp3 == 2) printf("\nType[ %i ] In Copied Tree Is Empty Now !!!\n",*choseActor);
    }

    void options(const int flag)
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        printf("\n[ 0 ] for return ( HOME PAGE )\n"
                       "[ 1 ] for confirm interested %s this actor\n"
                       "[ 2 ] for try again , I want %s other actor\n\n", str4[*tmp4-2], str4[*tmp4-2]);

        readInteger(tmp,0,2);

        if (*tmp == 1 && *tmp4 == 2)      printf("\nSuccessful Searching Process (-_-)\n");
        else if (*tmp == 1 && *tmp4 == 3) copy();
        else if (*tmp == 1 && *tmp4 == 4)
        {
            i=0;

            for(;i<threadPointer.size();++i) if(ptr==threadPointer[i])
            {
                printf("\nThis Actor Active Running In The Background !!!\n"
                               "[ 0 ] For Return ( HOME PAGE ) --Means--> ( No Delete It )\n"
                               "[ 1 ] For Inactivate From Background --Then--> ( Delete It )\n\n");

                readInteger(tmp,0,1);

                if(*tmp == 1)
                {
                    threadPointer[i]->StopAuto();
                    threadPointer[i]= nullptr;
                    threadPointer.erase(threadPointer.begin() + i);

                    threadRunning[i]->~thread();
                    threadRunning[i]= nullptr;
                    threadRunning.erase(threadRunning.begin() + i);

                    delet();
                }

                break;
            }

            if( i>=threadPointer.size() ) delet();
        }

        else if (*tmp == 1 && *tmp4 == 5)
        {
            i=0;

            for(;i<threadPointer.size();++i) if(ptr==threadPointer[i]) { printf("\nThis Actor Already Active Running In The Background !!!\n"); break; }

            if( i>=threadPointer.size() ) operations();
        }

        else if (*tmp == 2 && flag == 1) searchByName();
        else if (*tmp == 2 && flag == 2) searchByAddress();
        else if (*tmp == 2 && flag == 3) searchByPublicID();
        else if (*tmp == 2 && flag == 4) searchByPrivateID();
    }



    void copy()
    {
        // ( lvl0_Drug ) can take a copy from any ( lvl1_OneByOne , lvl1_ByQuantity , lvl2_BuyingByQuantity , lvl2_SellingByQuantity ) thanks inheritance and polymorphism and virtual pointer
        // ( this copy is independent from original ) thanks simulating copy constructor and virtual pointer

        printf("\n---------------------------------------------------------------------------------------------\n");

        if (treeCopied_name.searchByName(ptr->name)!=nullptr || treeCopied_address.searchByName(ptr->address)!=nullptr)
        {
            printf("\nthis actor ( Name or Address ) duplicated with another actor in Copied Version !!!\n\n");
            printf("[ 0 ] for return ( HOME PAGE )\n"
                           "[ 1 ] for copy another actor\n\n");

            readInteger(tmp,0,1);

            if(*tmp==1) search();
        }

        else
        {
            ptr->copied = true;
            ptr2 = ptr->copy();

            treeCopied_name.insertByName(ptr2->name, ptr2);//   ( master tree )   // insert this actor in ( Trie  :: treeCopied_name )    contain all actors type , data sorted by ( name )
            treeCopied_address.insertByName(ptr2->address, ptr2);                 // insert this actor in ( Trie  :: treeCopied_address ) contain all actors type , data sorted by ( address )

            treeCopied_ID[0].insertByValue(ptr2->publicID, ptr2);                 // insert this actor in ( BBST Tree  :: treeCopied[ 0 ] )     contain all actors type , data sorted by ( public ID )
            treeCopied_ID[ptr2->type].insertByValue(ptr2->privateID, ptr2);       // insert this actor in ( BBST Tree  :: treeCopied[ 1 : 4 ] ) contain once actor type , data sorted by ( private ID )

            ptr = ptr2 = nullptr;

            printf("\nSuccessful Copy Process (-_-)\n");
        }
    }



    void delet()
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        if (*tmp3 == 1 && tree_ID[0].searchByValue(ptr->publicID)!=nullptr)
        {
            treeCopied_ID[0].copiedKilled(ptr->publicID);

            tree_ID[0].deleteByValue(ptr->publicID);                 // release her pointers refer to  this actor from ( BBST Tree  :: treeCopied[ 0 ] )     contain all actors type , data sorted by ( public ID )
            tree_ID[ptr->privateID].deleteByValue(ptr->privateID);   // release her pointers refer to  this actor from ( BBST Tree  :: treeCopied[ 1 : 4 ] ) contain once actor type , data sorted by ( private ID )

            tree_address.deleteByName(ptr->address);                 // release her pointers refer to this actor from ( Trie  :: treeCopied_address ) contain all actors type , data sorted by ( address )
            tree_name.deleteByName(ptr->name);//   ( master tree )   // delete her pointers refer to this actor from ( Trie  :: treeCopied_name )    contain all actors type , data sorted by ( name )

            printf("\nSuccessful Delete From Original Version Process (-_-)\n");
        }

        else if (*tmp3 == 2 && treeCopied_ID[0].searchByValue(ptr->publicID)!=nullptr)
        {
            tree_ID[0].copiedKilled(ptr->publicID);

            treeCopied_ID[0].deleteByValue(ptr->publicID);                 // release her pointers refer to  this actor from ( BBST Tree  :: treeCopied[ 0 ] )     contain all actors type , data sorted by ( public ID )
            treeCopied_ID[ptr->privateID].deleteByValue(ptr->privateID);   // release her pointers refer to  this actor from ( BBST Tree  :: treeCopied[ 1 : 4 ] ) contain once actor type , data sorted by ( private ID )

            treeCopied_address.deleteByName(ptr->address);                 // release her pointers refer to this actor from ( Trie  :: treeCopied_address ) contain all actors type , data sorted by ( address )
            treeCopied_name.deleteByName(ptr->name);//   ( master tree )   // delete her pointers refer to this actor from ( Trie  :: treeCopied_name )    contain all actors type , data sorted by ( name )

            printf("\nSuccessful Delete From Copied Version Process (-_-)\n");
        }

        else
        {
            printf("\nthis actor not found , may be deleted before\n\n");
            printf("[ 0 ] for return ( HOME PAGE )\n"
                           "[ 1 ] for delete another actor\n\n");

            readInteger(tmp,0,1);
            if(*tmp==1) search();
        }

        ptr = ptr2 = nullptr;
    }



    void operations()
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        printf("\n[ 0 ] for return ( HOME PAGE )\n");

        if (ptr->type == 1)
        {
            printf("\n[ 1 ] For Increase Actor :: COUNT By ( 1 )\n\n"
                             "[ 2 ] For Decrease Actor :: COUNT By ( 1 )\n\n");
        }

        else if (ptr->type == 2)
        {
            printf("\n[ 1 ] For Increase Actor :: COUNT By ( %i )\n\n"
                             "[ 2 ] For Decrease Actor :: COUNT By ( %i )\n\n",*ptr->value,*ptr->value);
        }

        else if (ptr->type == 3)
        {
            printf("\n[ 1 ] For AutoIncrease Actor :: COUNT By ( %i ) Each ( %i ) Seconds In The Background\n\n"
                             "[ 2 ] For Decrease Actor :: COUNT By ( %i )\n\n",*ptr->value,ptr->getAuto(),*ptr->value);
        }

        else if (ptr->type == 4)
        {
            printf("\n[ 1 ] For Increase Actor :: COUNT By ( %i )\n\n"
                             "[ 2 ] For AutoDecrease Actor :: COUNT By ( %i ) Each ( %i ) Seconds In The Background\n\n",*ptr->value,*ptr->value,ptr->getAuto());
        }



        readInteger(tmp,0,2);

        if(*tmp != 0)
        {
            operations2();

            if(*tmp2!=0) operations();
        }
    }

    void operations2()
    {
        printf("\n---------------------------------------------------------------------------------------------\n");

        if (ptr!=nullptr && (ptr->type == 1 || ptr->type == 2 || ptr->type == 4) && *tmp == 1)       ptr->increase(), printf("\nSuccessful Running New Operation In Background (-_-)\n");
        else if (ptr!=nullptr && (ptr->type == 1 || ptr->type == 2 || ptr->type == 3) && *tmp == 2)  ptr->decrease(), printf("\nSuccessful Running New Operation In Background (-_-)\n");



        else if (ptr!=nullptr && ptr->type == 3 && *tmp == 1 && threadRunning.size() < maxThreads) // create new thread for ( ptr -> autoIncrease )
        {
            threadRun=new thread(&lvl0_Drug::increase, ptr);

            threadRunning.push_back(threadRun);
            threadPointer.push_back(ptr);

            ptr=nullptr;

            threadRun->detach();

            printf("\nSuccessful Running New Operation In Background (-_-)\n");
        }



        else if (ptr!=nullptr && ptr->type == 4 && *tmp == 2 && threadRunning.size() < maxThreads) // create new thread for ( ptr -> autoDecrease )
        {
            threadRun=new thread(&lvl0_Drug::decrease, ptr);

            threadRunning.push_back(threadRun);
            threadPointer.push_back(ptr);

            ptr=nullptr;

            threadRun->detach();

            printf("\nSuccessful Running New Operation In Background (-_-)\n");
        }



        else if(ptr!=nullptr)
        {
            printf("\nSorry (-_-) ,  system running now %i threads in background , this is a maximum available by this hardware\n\n"
                   "[ 0 ] for return ( HOME PAGE )\n"
                   "[ 1 ] for try another ( Increase , Decrease ) operation\n"
                   "[ 2 ] for go to  ( BACKGROUND PAGE ) , for stop any of the running operations and then run this operation\n\n",maxThreads);

            readInteger(tmp2,0,2);

            if(*tmp2==2) background();
        }
    }



    void background(bool FromHome=0)
    {
        printf("\n#############################################################################################\n");

        printf("\n\t\t\t\t( SYSTEM BACKGROUND PAGE )\n\t\t\t( System Running %i Thread In Background )\n\n",threadRunning.size());

        printf("\n#############################################################################################\n"
                         "#############################################################################################\n");

        if(threadPointer.size()) printf("\n[ 0 ] For Return ( HOME PAGE )\n");

        for (int i = 0; i < threadPointer.size(); ++i)
        {
            printf("\n=============================================================================================\n");


            if (threadPointer[i]->type == 3)      printf("[ %i ] For Stop Thread[ %i ] , That Increased ( Count ) By ( %i ) Automated , Through ( %i ) Seconds\n\n", i + 1, i + 1,
                    threadPointer[i]->getAutomated() * (*threadPointer[i]->value), threadPointer[i]->getAutomated() * threadPointer[i]->getAuto());

            else if (threadPointer[i]->type == 4) printf("[ %i ] For Stop Thread[ %i ] , That Decreased ( Count ) By ( %i ) Automated , Through ( %i ) Seconds\n\n", i + 1, i + 1,
                    threadPointer[i]->getAutomated() * (*threadPointer[i]->value), threadPointer[i]->getAutomated() * threadPointer[i]->getAuto());


            threadPointer[i]->print(0);
        }

        if(threadPointer.size()>1) printf("\n[ %i ] For Stop All Above Threads\n",threadPointer.size()+1);

        printf("\n#############################################################################################\n"
                         "#############################################################################################\n");



        if(threadPointer.size())
        {
            if(FromHome)                           *tmp2=2e9;
            else if(threadPointer.size() == 1) readInteger(tmp2,0,1);
            else if(threadPointer.size() > 1)  readInteger(tmp2,0,threadPointer.size()+1);



            if(*tmp2 == 0 && ptr!=nullptr) printf("\nUnSuccessful Running New Operation !!!\n");

            else if(*tmp2>=1 && *tmp2 <= threadPointer.size())
            {
                threadPointer[*tmp2 - 1]->StopAuto();
                threadPointer[i]= nullptr;
                threadPointer.erase(threadPointer.begin() + *tmp2-1);

                threadRunning[*tmp2 - 1]->~thread();
                threadRunning[i]= nullptr;
                threadRunning.erase(threadRunning.begin() + *tmp2-1);

                printf("\nThread[ %i ] Stopped And Become Inactive\n\n",*tmp2);

                *tmp2 = 0;
                operations2();
            }

            else if(*tmp2 > threadPointer.size())
            {
                for(i=0;i<threadPointer.size();++i)
                {
                    threadPointer[i]->StopAuto();
                    threadPointer[i]= nullptr;

                    threadRunning[i]->~thread();
                    threadRunning[i]= nullptr;
                }

                threadPointer.clear();
                threadRunning.clear();

                printf("\nAll Threads Which Was Active Running In Background ( Stopped And Become Inactive )\n\n",*tmp2);

                *tmp2 = 0;
                if(!FromHome) operations2();
            }
        }

        // *tmp2=0; // not used
    }



    bool printAll()
    {
        printf("\n[ 0 ] for return previous step\n"
               "[ 1 ] for operate in original actors\n"
               "[ 2 ] for operate in copied actors\n\n");
        readInteger(tmp, 0, 2);

        if(*tmp==1 && tree_ID->size()==0)            { printf("\nOriginal Tree Is Empty Now !!!\n"); return 0;}

        else if(*tmp==2 && treeCopied_ID->size()==0) { printf("\nCopied Tree Is Empty Now !!!\n"); return 0; }


        printf("\n---------------------------------------------------------------------------------------------\n");

        printf("[ 0 ]  for return ( HOME PAGE )\n\n"

                       "[ 1 ]  print actors sorted  by lexicographically order of Names\n"
                       "[ 2 ]  print actors sorted  by lexicographically order of Addresses\n\n"

                       "[ 3 ]  print actors sorted  by ascending  order of Public ID \n"
                       "[ 4 ]  print actors sorted  by descending order of Public ID \n"
                       "[ 5 ]  print actors by interested range (A : B) of Public ID\n\n"

                       "[ 6 ]  print actors sorted  by ascending  order of Priavte ID \n"
                       "[ 7 ]  print actors sorted  by descending order of Priavte ID \n"
                       "[ 8 ]  print actors by interested range (A : B) of Private ID\n\n"

                       "[ 9 ]  print number of actors that inserted in store\n"
                       "[ 10 ] print number of actors that deleted from store\n"
                       "[ 11 ] print number of actors that stored now\n\n\n"



                       "Special Prints: How The System Stored Actors ... \n\n"

                       "[ 12 ] print actors sorted by TreeShape order of Public ID\n"
                       "[ 13 ] print actors sorted by prefixes  order of Public ID\n"
                       "[ 14 ] print actors sorted by Postfixes order of Public ID\n\n"

                       "[ 15 ] print actors sorted by TreeShape order of Private ID\n"
                       "[ 16 ] print actors sorted by prefixes  order of Private ID\n"
                       "[ 17 ] print actors sorted by Postfixes order of Private ID\n");

        printf("---------------------------------------------------------------------------------------------\n\n");

        readInteger(tmp2,0,17);

        if(*tmp2 == 5 || *tmp2 == 8)
        {
            printf("\n( A ): ");
            readInteger(tmp3,0,2e9);

            printf("\n( B ): ");
            readInteger(tmp4,0,2e9);
        }



        if(*tmp!=0)
        {


            if (*tmp2 == 1 && *tmp == 1) tree_name.printLex();
            else if (*tmp2 == 2 && *tmp == 1) tree_address.printLex();
            else if (*tmp2 == 3 && *tmp == 1) tree_ID[0].printAscending();
            else if (*tmp2 == 4 && *tmp == 1) tree_ID[0].printDescending();
            else if (*tmp2 == 5 && *tmp == 1) tree_ID[0].printRange(*tmp3,*tmp4);
            else if (*tmp2 == 6 && *tmp == 1) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n",i); tree_ID[i].printAscending();  printf("\n\n\n"); }
            else if (*tmp2 == 7 && *tmp == 1) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n",i); tree_ID[i].printDescending(); printf("\n\n\n"); }
            else if (*tmp2 == 8 && *tmp == 1) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n",i); tree_ID[i].printRange(*tmp3,*tmp4);   printf("\n\n\n"); }
            else if (*tmp2 == 9 && *tmp == 1)  tree_name.inserted();
            else if (*tmp2 == 10 && *tmp == 1) tree_name.deleted();
            else if (*tmp2 == 11 && *tmp == 1) tree_name.size();

            else if (*tmp2 == 12 && *tmp == 1) tree_ID[0].printTreeShape();
            else if (*tmp2 == 13 && *tmp == 1) tree_ID[0].printPrefixes();
            else if (*tmp2 == 14 && *tmp == 1) tree_ID[0].printPostfixes();
            else if (*tmp2 == 15 && *tmp == 1) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n",i); tree_ID[i].printTreeShape();  printf("\n\n\n"); }
            else if (*tmp2 == 16 && *tmp == 1) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n",i); tree_ID[i].printPrefixes();   printf("\n\n\n"); }
            else if (*tmp2 == 17 && *tmp == 1) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n",i); tree_ID[i].printPostfixes();  printf("\n\n\n"); }



            else if (*tmp2 == 1 && *tmp == 2) treeCopied_name.printLex();
            else if (*tmp2 == 2 && *tmp == 2) treeCopied_address.printLex();
            else if (*tmp2 == 3 && *tmp == 2) treeCopied_ID[0].printAscending();
            else if (*tmp2 == 4 && *tmp == 2) treeCopied_ID[0].printDescending();
            else if (*tmp2 == 5 && *tmp == 2) treeCopied_ID[0].printRange(*tmp3, *tmp4);
            else if (*tmp2 == 6 && *tmp == 2) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n", i); treeCopied_ID[i].printAscending();  printf("\n\n\n"); }
            else if (*tmp2 == 7 && *tmp == 2) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n", i); treeCopied_ID[i].printDescending(); printf("\n\n\n"); }
            else if (*tmp2 == 8 && *tmp == 2) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n", i); treeCopied_ID[i].printRange(*tmp3, *tmp4);   printf("\n\n\n"); }
            else if (*tmp2 == 9 && *tmp == 2)  treeCopied_name.inserted();
            else if (*tmp2 == 10 && *tmp == 2) treeCopied_name.deleted();
            else if (*tmp2 == 11 && *tmp == 2) treeCopied_name.size();

            else if (*tmp2 == 12 && *tmp == 2) treeCopied_ID[0].printTreeShape();
            else if (*tmp2 == 13 && *tmp == 2) treeCopied_ID[0].printPrefixes();
            else if (*tmp2 == 14 && *tmp == 2) treeCopied_ID[0].printPostfixes();
            else if (*tmp2 == 15 && *tmp == 2) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n",i); treeCopied_ID[i].printTreeShape();  printf("\n\n\n"); }
            else if (*tmp2 == 16 && *tmp == 2) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n",i); treeCopied_ID[i].printPrefixes();   printf("\n\n\n"); }
            else if (*tmp2 == 17 && *tmp == 2) for (i = 1; i <= 4; ++i) { printf("\nType[ %i ]\n",i); treeCopied_ID[i].printPostfixes();  printf("\n\n\n"); }
        }

        return 0;
    }
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// main()
int main()
{
    DrugSystem client;
    client.chatbot();



    // This is whole system , simulating ( Drug System ) --> ( Buy & Sell ) by different ways ( one by one,  by quantity,  by quantity but automatic Increase,  by quantity with automatic decrease )

    // I created system collected all tasks required of me in the interview, And added some of other useful features created it from scratch like ( Balanced Binary Search Tree )

    // which give you ability to access and manipulating the data   by ( Name   or   Address ) using ( Trie )   ,   by ( Public ID   or   Private ID ) using ( Balanced Binary Search Tree )
    // in Order ( N ) , Worst case O( N ) == length of the tree     -->     O( Trie ) == longest word in tree   ,   O( Balanced Binary Search Tree ) == log( Size of tree )

    // so we store the data ( once ) and can access using ( 4 trees of pointers ) by ( 4 methods )   -->   ( Name , Address , Public ID , Private ID )
    // and make ( one ) tree is a ( Master Tree ) --> can delete actual stored data in memory , and other ( 3 ) trees only release her pointers to null

    // maximum threads can active running in the background depending on the hardware , so the system asks the device about its ability

    // Make the system creation any stored data with two main conditions ( unique Name   ,   unique Address )



    system("pause");
    return 0;
}
