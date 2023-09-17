# syntax

1. Variable

```rust  <!--syntax highlighting hack-->
int : 2; "int";

b : 2.1; "decimal";

this is
also an		int : 123; "space, tab, newline can be a part of variable";

xin chào : "hello"; "utf-8 is also accepted";

mảng : [123, 23.4 ,  "hmm"]; "array";

mảng[2]; "will return \"hmm\" ";

```

2. Function

```rust
{}; "a empty function";

{2}; "thís function will return 2 when called";

{temp : 1+2+3; temp+3+4}; "multiple statement in one function";

func : {12+23}; "function can be assigned to a variable";

func(12); "And called";
```

3. Core function \
All core function are inside the "shared" scope
That environment is accessible via "@" symbol

|Function|Usage|
|-|-|
|if(condition,then,else)|Return  `then` if `condition` is not null and not 0. If not, return `else`|
|while(block)|Execute `block` until it return null or 0|
|try(block,handler)|Execute `block`,if there is an exception, execute `handler` with the exception as argument|
|throw(exception)|Throw `exception`|
|args()|Return arguments of current scope as an array|
|prev()|Return the previous scope of current one. Variables in the that scope can be accessed like a member|
|this()|Return the nearest object|
|self()|Return the nearest function|
