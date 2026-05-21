# Virtual Base Initialization

Let's look back at our diamond hierarchy:

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

This time, let's say `Base` requires an ID number in its constructor:

```cpp
class Base {
public:
    Base(int id) { /* ... */ }
};
```

If normal constructor rules applied, both `Der1` and `Der2` would try to pass a value to `Base` when a `Join` object is created:

```cpp
class Der1 : public virtual Base {
public:
    Der1() : Base(10) {}
};

class Der2 : public virtual Base {
public:
    Der2() : Base(20) {}
};
```

Because there is **only one shared instance** of `Base` inside `Join`, C++ faces a contradiction. Should the shared `Base` be initialized with `10` or `20`?

To resolve this conflict, C++ strips `Der1` and `Der2` of their right to initialize `Base` when they are part of a larger, more-derived class. Instead, `Join` must initialize `Base` **directly**.

If you try to write `Join` without explicitly calling `Base`'s constructor, the compiler will yell at you (unless `Base` has a default constructor with no arguments).

You have to pass the parameters all the way from the bottom of the diamond:

```cpp
class Join : public Der1, public Der2 {
public:
    // Even though Base is a grandparent, Join must call its constructor directly
    Join(int base_id) : Base(base_id), Der1(), Der2() { /* ... */ }
};
```

Here is the wildest part about this rule: the constructor code written inside `Der1` and `Der2` that looks like it calls `Base(10)` or `Base(20)` is **completely ignored** by the CPU when creating a `Join` object.

- If you instantiate `Der1` directly (`Der1* d = new Der1();`), `Der1` *is* the most-derived class, so it successfully initializes `Base` with `10`.
- If you instantiate `Join` directly, `Join` is the most-derived class. The initialization paths for `Base` from `Der1` and `Der2` are silently bypassed, and only `Join`'s call to `Base` executes.

Now imagine someone creates a new class:

```cpp
class UltimateJoin : public Join {
public:
    UltimateJoin(int base_id) : Base(base_id), Join(base_id) {}
};
```

Even though `UltimateJoin` is a *great-grandchild* of `Base`, because it is now the "most-derived class", **it** is now responsible for initializing `Base`. The initialization code you carefully wrote in `Join` is now the one being ignored! Every single time someone inherits further down the chain, they must propagate those `Base` constructor arguments forward.

Because of this mandatory delegation, C++ developers usually follow two unwritten rules to keep their sanity when using virtual inheritance:

1. **Keep Virtual Bases Stateless:** Try to make your virtual base classes pure interfaces with no member variables and no constructors (or only a default, parameterless constructor). If `Base` doesn't require any arguments, the most-derived class handles the initialization automatically behind the scenes.
2. **Order of Destruction:** Destructors face the exact same rule in reverse. The most-derived class (`Join`) will destroy `Base` *exactly once*, and it will do so at the very end, after `Der1` and `Der2` have already been destroyed.