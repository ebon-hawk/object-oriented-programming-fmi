# Abstract vs. Virtual Base Classes

A quick comparison to distinguish these two often-confused concepts.

| Feature | Abstract Base Class | Virtual Base Class |
| --- | --- | --- |
| **Primary Purpose** | To enforce a strict interface and provide a common foundation for derived classes. | To resolve ambiguity and prevent duplicate data in complex inheritance (diamond problem). |
| **Instantiation** | Cannot be instantiated directly. | Can be instantiated normally (provided it has no abstract methods). |
| **Mechanism** | Achieved using pure virtual functions (e.g., `virtual void draw() = 0`). | Achieved using the `virtual` keyword during inheritance (e.g., `class B : virtual public A`). |
| **Key Benefit** | Consistency across child classes. | Memory efficiency and single sub-objects. |

## 1. Abstract Base Classes

An ABC defines a template for other classes. It establishes a "must-have" contract for methods.

If a child class inherits from an ABC but fails to override its pure virtual methods, that child class also becomes abstract.

```cpp
class AbstractShape {
public:
    // Pure virtual function makes this class abstract
    virtual void draw() = 0;
};
```

## 2. Virtual Base Classes

Primarily a C++ feature, virtual inheritance ensures that a derived class only receives a single, shared copy of a base class's data, no matter how many paths lead back to it.

```text
			Base
			/  \
		   /    \
		  /      \
		Der1    Der2
		  \      /
		   \    /
		    \  /
			Join
```

Without the `virtual` keyword, a `Join` object contains **two separate** instances of `Base`, leading to ambiguity errors when accessing members or casting pointers.

By marking the intermediate parents as virtual, we consolidate the hierarchy.

```cpp
class Base { protected: int data_; };

// Virtual keyword goes here
class Der1 : public virtual Base { /* ... */ };
class Der2 : public virtual Base { /* ... */ };

class Join : public Der1, public Der2 {
public:
    // Success: data_ is unique
    void method() { data_ = 1; }
};
```

## 3. Advanced Pattern: Cross-Delegation

"Cross-delegation" is a powerful technique where one sibling in a diamond hierarchy provides the implementation for a virtual function called by another sibling.

```cpp
class Base {
public:
    virtual void foo() = 0;
    
    virtual void bar() = 0;
};

class Der1 : public virtual Base {
public:
    // Calls bar() without knowing who implements it
    void foo() override { bar(); }
};

class Der2 : public virtual Base {
public:
    void bar() override { /* Real implementation... */ }
};

class Join : public Der1, public Der2 { /* ... */ };
```

When `Join` is instantiated, the compiler builds a unified **vtable**.

- **The Call:** `Der1::foo()` is executed.
- **The Internal Dispatch:** Inside `foo()`, a call to `bar()` is made.
- **The Resolve:** The compiler looks at the vtable of the complete object (`Join`).
- **The Crossover:** It finds the final override in `Der2`, allowing logic to jump across the diamond branches.

### Practical Use Case: GUI Library

- `Base` defines a generic `Component` that needs to be drawn and clicked.
- `Der1` implements the behavior (a `Button` component). Its `foo()` might be `click()`, which internally calls `refresh_graphics()`. But `Button` doesn't know how to draw graphics on a specific OS.
- `Der2` implements the OS rendering (a `WindowsGraphics` backend that knows how to implement `refresh_graphics()`).
- `Join` simply glues them together (`class WindowsButton : public Button, public WindowsGraphics { /* ... */ };`).

Because of cross-delegation, your `Button` logic happily triggers `WindowsGraphics` drawing code without ever having to be recompiled or linked directly against Windows-specific headers.

## 4. Summary & Best Practices

- Use **abstract classes** when you want to define a common interface that cannot exist as a standalone object.
- Use **virtual inheritance** only when a "diamond" structure is likely. It adds a slight runtime overhead due to pointer indirection.
- **The "Overlap":** A class is frequently both. If a virtual base class contains pure virtual functions, it is both an ABC and a VBC.
- **Initializers:** Remember that in virtual inheritance, the **most-derived class** (e.g., `Join`) is responsible for calling the constructor of the virtual base (`Base`).

## 5. Further Reading & Resources

- [C++ Core Guidelines: Classes and Class Hierarchies](https://cpp-core-guidelines-docs.vercel.app/class)
- [Multiple Base Classes](https://learn.microsoft.com/en-us/cpp/cpp/multiple-base-classes?view=msvc-170)
- [Multiple Inheritance: The Good, the Bad, and the Ugly](https://www.geeksforgeeks.org/multiple-inheritance-in-c/)
- [Multiple and Virtual Inheritance](https://isocpp.org/wiki/faq/multiple-inheritance)
- [The Diamond Problem in C++](https://www.learncpp.com/cpp-tutorial/virtual-base-classes/)