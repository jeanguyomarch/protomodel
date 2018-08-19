# protomodel

Protomodel is a [flatbuffers][2] compiler-assistant that parses the
[flatbuffers' IDL][3] to extract its significant information and make them
available to a templating engine. Information contained within a flatbuffers
interface can therefore be transposed via templating as another form to serve a
different purpose.

For instance, this allows to easily translate data contained within a TOML file
into an in-memory flatbuffers representation. 

## Usage

Protomodel requires a flatbuffers interface, which it will parse and extract
significant information, that will be used as a templating context for a list
of templates (provided with the `-t` option). Each template must be associated
with an output file (with `-o`).

```bash
protomodel <flatbuffers_model.fbs> -t <template> -o <templated_output>
```

## Building protomodel

Protomodel and its tools are built using the [CMake][1] build system.  Prior
bootstrapping CMake, protomodel build dependencies shall be installed first.
To do so, just run `scripts/install-deps.sh` from the top source directory.
The following code snippet show how to build and install a release-build of
protomodel:

```bash
sh scripts/install-deps.sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cmake --build . --target install # Probably as root (i.e. via sudo)
```

## Data Model

Protomodel parses a [flatbuffers][2] interface description and allows to
template input files with the data collected from this file. The following
data model is exposed to the templating engine.

### Root Node (top-level)

|*Value*     |*Type*     |*Description*                                       |
|------------|-----------|----------------------------------------------------|
|`includes`  |`[Include]`|C++ includes                                        |
|`tables`    |`[Table]`  |Tables declared in the flatbuffers interface        |
|`root_type` |`string`   |Name of the flatbuffers root type                   |
|`model_name`|`string`   |Root namespace of the data model                    |
|`magic`     |`string`   |Magic declared in the flatbuffers interface         |


### Include Type

|*Value*         | *Type* |*Description*                                      |
|----------------|--------|---------------------------------------------------|
|`name`          |`string`|Name of the flatbuffers root type                  |
|`name_uppercase`|`string`|Name in uppercases                                 |


### Table Type

|*Value*        | *Type*       |*Description*                                 |
|---------------|--------------|----------------------------------------------|
|`table_name`   |`string`      |Name of the flatbuffers Table                 |
|`table_type`   |`string`      |Type of the flatbuffers Table                 |
|`values`       |`[TableValue]`|Values contained within the table             |
|`repeated_values`|`[TableValue]`|List of Values contained within the table   |
|`objects`      |`[TableObject]`|Objects contained within the table           |
|`repeated_objects`|`[TableObject]`|List of Objects contained within the table|
|`maps`         |`[TableMap]`  |Key-identified objects within the table       |


### TableValue Type

|*Value*    | *Type*        |*Description*                                    |
|-----------|---------------|-------------------------------------------------|
|`name`     |`string`       |Name of the flatbuffers Table                    |
|`py_name`  |`string`       |Python name of the type                          |
|`type`     |`[TableValue]` |Values contained within the table                |
|`required` |`[TableValue]` |List of Values contained within the table        |
|`at_least` |`integer`      |Minimal value allowed                            |
|`at_most`  |`integer`      |Maximal value allowed                            |
|`has_range`|`boolean`      |Tell whether the value accepts a numerical range |


### TableMap Type

|*Value*    | *Type*        |*Description*                                    |
|-----------|---------------|-------------------------------------------------|
|`name`     |`string`       |Name of the flatbuffers Table                    |
|`py_name`  |`string`       |Python name of the type                          |
|`key_name` |`string`       |Name of the field that acts as a key             |
|`key_type` |`string`       |Type of the field that acts as a key             |
|`value_type`|`string`      |Type of the values within the map                |
|`value_obj_type`|`string`  |C++ native object type name                      |
|`at_least` |`integer`      |Minimal value allowed                            |
|`at_most`  |`integer`      |Maximal value allowed                            |


### TableMap Type

|*Value*    | *Type*        |*Description*                                    |
|-----------|---------------|-------------------------------------------------|
|`name`     |`string`       |Name of the flatbuffers Table                    |
|`py_name`  |`string`       |Python name of the type                          |
|`key_name` |`string`       |Name of the field that acts as a key             |
|`key_type` |`string`       |Type of the field that acts as a key             |
|`value_type`|`string`      |Type of the values within the map                |
|`value_obj_type`|`string`  |C++ native object type name                      |
|`at_least` |`integer`      |Minimal value allowed                            |
|`at_most`  |`integer`      |Maximal value allowed                            |


### TableObject Type

|*Value*    | *Type*        |*Description*                                    |
|-----------|---------------|-------------------------------------------------|
|`name`     |`string`       |Name of the flatbuffers Table                    |
|`py_name`  |`string`       |Python name of the type                          |
|`type`     |`string`       |Type of the object                               |
|`obj_type` |`string`       |C++ native type of the object                    |
|`required` |`boolean`      |True if the object is required                   |
|`at_least` |`integer`      |Minimal value allowed                            |
|`at_most`  |`integer`      |Maximal value allowed                            |


## Templating Engine

Protomodel relies on the [mustache][5] templating specification. It uses the
[mstch][4] implementation.


[1]: https://cmake.org/
[2]: https://google.github.io/flatbuffers/
[3]: https://google.github.io/flatbuffers/flatbuffers_guide_writing_schema.html
[4]: https://github.com/no1msd/mstch
[5]: http://mustache.github.io/
