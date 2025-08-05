#include "Examples.hpp"

#include "BicycleImpl.hpp"
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"

using namespace mybicycles;

namespace
{
    const bool BICYCLE_DEBUG_PRINTS = true;
}

void testUniquePtr_DefaultDeleter()
{
    std::cout << "============== UniquePtr test: default deleter ============== " << std::endl;
    {
        // Ctors, dtors
        BicycleImpl* giant = new BicycleImpl("Giant", BICYCLE_DEBUG_PRINTS);
        UniquePtr<BicycleImpl> up1(giant);
        UniquePtr<BicycleImpl> up2(new BicycleImpl("Cannondale", BICYCLE_DEBUG_PRINTS));

        // Move assignment
        // up1 = up2; // won't compile
        up1 = std::move(up2);

        // Move ctor
        // UniquePtr<Bicycle> up3(up1); // won't compile
        UniquePtr<BicycleImpl> up3(std::move(up1));

        BicycleImpl* cannondale = up3.release();
        delete cannondale;
        // Check resetting nullptr with nullptr:
        up3.reset();
        // Check resetting nullptr with raw pointer:
        up3.reset(new BicycleImpl("Bianchi", BICYCLE_DEBUG_PRINTS));
        // Check resetting with rvalue ref:
        up3.reset(std::move(UniquePtr<BicycleImpl>(new BicycleImpl("Fuji Bikes", BICYCLE_DEBUG_PRINTS))));

        UniquePtr<BicycleImpl> up4(new BicycleImpl("Santa Cruz", BICYCLE_DEBUG_PRINTS));
        std::cout << "up3 says: "; up3->ringBell();
        std::cout << "up4 says: "; up4->ringBell();
        up3.swap(up4);
        std::cout << "After swap:" << std::endl;
        std::cout << "up3 says: "; (*up3).ringBell(); // TODO: precendence
        std::cout << "up4 says: "; (*up4).ringBell();
        UniquePtr<BicycleImpl>::swap(up3, up4);
        std::cout << "After swap again:" << std::endl;
        std::cout << "up3 says: "; up3->ringBell();
        std::cout << "up4 says: "; up4->ringBell();

        up3.reset();
        if (!up3)
        {
            std::cout << "up3 is null after reset, as expected" << std::endl;
        }
        if (up4 && up4 != nullptr) // checking operators together to save space
        {
            std::cout << "up4 is not null; address of held resource: " << up4.get() << std::endl;
        }
        if (up4 == nullptr)
        {
            std::cout << "never here! up4 is not null" << std::endl;
        }
    }
    std::cout << "============================================================= " << std::endl;
}

void testUniquePtr_CustomDeleter()
{
    struct BoxOfThings
    {
        ~BoxOfThings() { std::cout << __PRETTY_FUNCTION__ << std::endl; }

        std::string name;
        int thingOne;
        long thingTwo;
        long thingThree;
    };

    std::cout << "============== UniquePtr test: custom deleter ============== " << std::endl;
    {
        {
            BoxOfThings* box1 = (BoxOfThings*)malloc(sizeof(BoxOfThings));
            UniquePtr<BoxOfThings, CDeleter<BoxOfThings>> up1(box1);

            BoxOfThings* box2 = nullptr;
            UniquePtr<BoxOfThings, CDeleter<BoxOfThings>> up2(box2);
            // OUPUT:
            // I am a custom deleter
            // Submitted resource is already null
            // I am a custom deleter
        }

        UniquePtr<BoxOfThings, CDeleter<BoxOfThings>> up3((BoxOfThings*)malloc(sizeof(BoxOfThings)));
        up3.reset((BoxOfThings*)malloc(sizeof(BoxOfThings)));
        if (up3 != nullptr)
        {
            up3->name = "BoxOfMatches";
            std::cout << "up3 says: " << (*up3).name << std::endl;
        }
        // OUTPUT:
        // I am a custom deleter
        // up3 says: BoxOfMatches
        // I am a custom deleter
    }
    std::cout << "============================================================= " << std::endl;
}

void testUniquePtr_RawArray_CustomDeleter()
{
    std::cout << "======= UniquePtr test: custom deleter with raw arrays ====== " << std::endl;
    {
        BicycleImpl* bicycles = new BicycleImpl[3]{{"Raleigh", BICYCLE_DEBUG_PRINTS},
                                                   {"Scott", BICYCLE_DEBUG_PRINTS},
                                                   {"Trek", BICYCLE_DEBUG_PRINTS}};
        UniquePtr<BicycleImpl, ArrayDeleter<BicycleImpl>> up1(bicycles);

        // OUTPUT:
        // mybicycles::Bicycle::Bicycle(std::string) | Raleigh
        // mybicycles::Bicycle::Bicycle(std::string) | Scott
        // mybicycles::Bicycle::Bicycle(std::string) | Trek
        // mybicycles::Bicycle::~Bicycle() | Trek
        // mybicycles::Bicycle::~Bicycle() | Scott
        // mybicycles::Bicycle::~Bicycle() | Raleigh
    }

    const size_t BIKES_NUM = 2;
    UniquePtr<BicycleImpl, ArrayDeleter<BicycleImpl>> up2(new BicycleImpl[BIKES_NUM]{{"Colnago"}, {"Specialized"}});
    for (int i = 0; i < BIKES_NUM; i++)
    {
        up2.get()[i].ringBell(); // Ugly syntax, I know
    }
    up2.reset();
    // OUTPUT:
    // mybicycles::Bicycle::Bicycle(std::string) | Colnago
    // mybicycles::Bicycle::Bicycle(std::string) | Specialized
    // Caution! Bike Colnago is on its way!
    // Caution! Bike Specialized is on its way!
    // mybicycles::Bicycle::~Bicycle() | Specialized
    // mybicycles::Bicycle::~Bicycle() | Colnago

    std::cout << "============================================================= " << std::endl;
}

void testUniquePtr_OutputTest()
{
    std::cout << "===== UniquePtr test: ostream operator overloading test ===== " << std::endl;
    {
        UniquePtr<BicycleImpl> up1;
        std::cout << "Null UniquePtr: " << up1 << std::endl;

        BicycleImpl* giant = new BicycleImpl("Giant", BICYCLE_DEBUG_PRINTS);
        UniquePtr<BicycleImpl> up2(giant);
        std::cout << "Non-null UniquePtr: " << up2 << std::endl;
    }
    std::cout << "============================================================= " << std::endl;
}

void testUniquePtr_MakeUnique()
{
    std::cout << "================= UniquePtr test: makeUnique ================= " << std::endl;
    {
        UniquePtr<BicycleImpl> up1 = makeUnique<BicycleImpl>("Colnago", 59, 59);
        std::cout << "Made a bicycle: " << *up1 << std::endl;
    }
    std::cout << "============================================================= " << std::endl;
}
