# NO-EXCEPT C++ Library

This is a mostly template library for C++, but it's not a header only library. The aim of this project, is to simply _be_, nothing more. But, if you don't like the design of the C++ standard library, and you want to avoid it like the plague, you can try using this as a replacement (In about 10 to 15 years when I actually finish the thing).

## Installation

This library replaces the standard new and delete operators with noexcept versions, that return a null pointer instead of throwing. This requires the library to be linked before libstdc++, or libsupc++, so it can override the standard new and delete operators.

## API Reference

Coming not so soon!

## Contributors

Nope, this is my library, and I do it how I want to. All bugs are intentional until further notice!

## License

Licensed under the MIT License, you can read about it here: https://opensource.org/licenses/MIT
