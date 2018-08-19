# Protomodel Tools

Protomodel provides second-class tools to manipulate flatbuffers models. They
are provided AS-IS, and are under the MIT License.

## protomerge

Flatbuffers does not support to extend a `table` once it is defined. Protomerge
provides a language extension to flatbuffers in the sense that a flatbuffers
model can be generated from two different definition of the same table.  This
simulates a pseudo Table inheritence (mixin).

If the extension file specifies a table that was not defined in the base
flatbuffers model, then this table will be added to the final model without
question.

If the extension file specifies a table that already exists in the base
flatbuffers model, then it **must** be prefixed by `@extend` so its fields will
be *appended* to the existing table. Otherwise, protomerge will refuse to merge
the two flatbuffers file. No checks are performed if the fields are re-defined.
Flatc will eventually fail if used on a faulty generated model file.

```fbs
// In the base file
table Config {
  number: int;
}

// In the extension file
@extends table Config {
  name: string;
}

// Merged file
table Config {
  // Fields from the base model
  number: int;

  // Fields from the extension model
  name: string;
}
```
