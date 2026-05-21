# Virtual Inheritance Ambiguity

Consider the following example:

```cpp
class Base {
public:
    virtual void foo() = 0;
    
    virtual void bar() = 0;
};

class Der1 : public virtual Base {
public:
    void foo() override { bar(); }
    
    void bar() override { /* ... */ }
};

class Der2 : public virtual Base {
public:
    void bar() override { /* ... */ }
};

class Join : public Der1, public Der2 { /* ... */ };

int main() {
    Join* p1 = new Join();
    Der1* p2 = p1;
    Base* p3 = p1;
    p1->foo();
    p2->foo();
    p3->foo();
}
```

If both `Der1` and `Der2` provide their own implementation of `bar()`, you hit a classic compilation error called **ambiguity**.

When `Der1::foo()` executes and looks up `bar()` in the vtable for `Join`, the compiler needs to know what the final, definitive override of `bar()` is for a `Join` object.

If both `Der1` and `Der2` provide an implementation, they are sitting at the exact same "level" of the inheritance hierarchy. Neither one has authority over the other. The compiler sees two competing implementations for the shared `Base::bar()` subobject and bails out.

To make this compile, `Join` (the most-derived class) must step in and break the tie. It has to explicitly state which `bar()` it wants to use, or provide its own brand-new override.

### Option 1 → Explicitly Delegate to One of the Parents

If you want `Join` to use `Der2`'s version of `bar`, you must override `bar()` in `Join` and explicitly scope it:

```cpp
class Join : public Der1, public Der2 {
public:
    // Disambiguate by explicitly choosing Der2's version
    void bar() override { Der2::bar(); }
};
```

### Option 2 → Provide a Completely New Implementation

Alternatively, `Join` can just do its own thing, which overrides both parents simultaneously:

```cpp
class Join : public Der1, public Der2 {
public:
    void bar() override {
        // Brand new logic for Join
    }
};
```

### The One Exception → The Rule of Dominance

There is one really interesting scenario where having `bar()` in both `Der1` and `Der2` *is* allowed without `Join` having to fix it. This happens if one of the classes actually inherits from the other, creating a clear "winner".

Imagine this layout instead:

```cpp
class Der1 : public virtual Base { /* ... */ };
class Der2 : public Der1 { /* ... */ };
```

Under C++'s **Rule of Dominance**, if name lookup finds two overrides for the same virtual base class function, but one of those overriding classes is derived from the other (i.e., `Der2` is a child of `Der1`), the more-derived class **dominates**.

The compiler will automatically choose `Der2::bar()` because it is considered "more specific" than `Der1::bar()`, and no ambiguity error will be thrown.