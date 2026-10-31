#include <iostream>

//entities
template<typename T>
struct CRTPEntity
{
    void doSomething() {
        static_cast<T*>(this)->doImplemented();
    }
};

struct CRTPArcher : CRTPEntity<CRTPArcher>
{
    void doImplemented(){std::cout << "I'm an Archer" << std::endl;}
};

struct CRTPKnight : CRTPEntity<CRTPKnight>
{
    void doImplemented(){std::cout << "I'm an CRTPKnight" << std::endl;}
};

struct CRTPOrc : CRTPEntity<CRTPOrc>
{
    void doImplemented(){std::cout << "I'm an CRTPOrc" << std::endl;}
};

struct CRTPOrcArcher : CRTPEntity<CRTPOrcArcher>
{
    void doImplemented(){std::cout << "I'm an CRTPOrcArcher" << std::endl;}
};

//usage
template<typename T>
void makeIt(CRTPEntity<T>& b){
    b.doSomething();
}


int main()
{
    CRTPArcher archer;
    CRTPKnight knight;
    CRTPOrc orc;
    CRTPOrcArcher orcArcher;

    makeIt(archer);
    makeIt(knight);
    makeIt(orc);
    makeIt(orcArcher);    

    return 0;
}
