Builtin functions
=================

These functions are built into Lily, and are always available from any scope without doing any importing.

# Functions

`print[A](input: A)`

Convert the value given into a string. Currently, this is done through an internal stringification process.

If Lily is running from Apache, then this will add content to the current page.

In any other case, this will write to stdout.


`printfmt(format: string, args: List[Dynamic]...)`

This is like print, except that it allows `%` modifiers to indicate replacements. 

The following modifiers are supported:

* `%d`: Convert an integer value.

* `%f`: Convert a double value.

* `%s`: Convert anything to a string. This is currently a work-in-progress, as Lily has a crude mechanism for converting anything into a string. However, because Lily lacks virtual calls, it is unwise to use this for anything but built-in types such as lists and hashes.

If there are more format specifiers than arguments, then `ValueError` is raised.

This function is deprecated. It will be superceded by string interpolation.


`calltrace : List[String])`

This returns a description of the current call stack, with each string representing a single call. Lily's call stack includes native calls, as well as calls to API functions (such as `integer::to_s`)

If this frame comes from something in native Lily code, then it takes the form of

`from <filename>:<linenum>: in <function>`.

If it comes from foreign code (such as `List::select`), then it takes the form of

`from [C]: in <function>`.


# Variables

`stdout: File`

This is a wrapper around C's stdout.

`stderr: File`

This is a wrapper around C's stderr.

`stdin: File`

This is a wrapper around C's stdin.
