# Language Ideas to consider

## Data Objects

```
data object ID {
  x -> number = 10; // stores data together in object
}
```

## Structs / Objects

```
object/struct ID {
  x -> number; // blueprint for object (similar to Go)
}
```

Can extent struct with methods outside of the structs main definition.

```
...
func ID.foo() -> void { out "hello"; } // now ID contains method foo(). 
```
