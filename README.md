# zap-toml

A TOML parsing library for [Zap](https://github.com/thezaplang/zap), backed by
[tomlc17](https://github.com/cktan/tomlc17) through FFI.

.

## Layout

| Path                | Purpose                                              |
|---------------------|------------------------------------------------------|
| `tomlc17/`          | Vendored tomlc17 C library                           |
| `csrc/toml_shim.c`  | Pointer-based C wrapper around tomlc17               |
| `build_lib.sh`      | Compiles tomlc17 + shim into `lib/libztoml.a`        |
| `src/ffi.zp`        | Raw `ext fun` declarations for the `zt_*` symbols    |
| `src/toml.zp`       | High-level Zap API (`Document`, `Value`)             |
| `src/main.zp`       | Demo program                                         |

## Building

The native static library must be built once before compiling Zap code:

```sh
./build_lib.sh        # produces lib/libztoml.a
thor run              # builds src/main.zp and runs the demo
```



`-Llib -lztoml` links the native library.

## Usage

Errors use Zap's failable functions. Parsing, lookups, and typed accessors
return `T!toml.TomlError`; propagate with `?` or handle locally with
`or <fallback>` / `or err { ... }`.

```zap
import "std/io" { println, printInt };
import "toml";

fun load() Int!toml.TomlError {
    var doc: toml.Document = toml.parseString("port = 8080")?;
    var port: Int64 = doc.root().getInt("port")?;
    printInt(port as Int);
    doc.close();               // releases the whole parsed tree
    return 0;
}

fun main() Int {
    return load() or err {
        println(err.message);  // err is a toml.TomlError
        1
    };
}
```


### Error handling

- `toml.TomlError` is an `@error struct` with `kind: ErrorKind` and
  `message: String`.
- `ErrorKind` is `Parse`, `NotFound`, `WrongType`, or `IndexOutOfRange`.
- Propagate with `?`, supply a default with `or <value>` (e.g.
  `v.asInt() or 30`), or branch on `err.kind` inside `or err { ... }`.
- `?` propagates a full expression; it does not chain mid-expression
  (`v.get(k)?.asInt()?` is not valid). Bind the intermediate `Value` to a
  `var` and unwrap each step, as the demo's helpers do.

## Memory model

A `Document` owns the entire parsed tree. Every `Value` borrows into that tree
and stays valid only until `doc.close()`. Always call `doc.close()` exactly
once per parsed `Document` (a parse *failure* frees the tree for you before
failing, so only close a `Document` you successfully obtained).
