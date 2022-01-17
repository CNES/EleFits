# Documentation guidelines

**! Work in progress !**

[TOC]

## Project documentation

TODO

## Module documentation

TODO

## Namespace documentation

TODO

## Class documentation


Class-level documentation is made of the following sequence of sections (if applicable).
Corresponding Doxygen commands or aliases, if any, are provided between braces.

* Parent module (`@ingroup`)
* Class declaration
* Brief description (`@brief`)
* Template parameters (`@tparam`)
* Detailed description (`@details`)
* Warnings (`@warning`)
* Template specializations (`@tspecialization` TODO)
* Satisfied requirements (`@satisfies`)
* Examples (`@par_example`)
* See also (`@see`)
* Notes (`@note`)
* Type definitions
* Inherited type definitions
* Member variables
* Inherited member variables
* Methods
* Inherited methods
* Related functions
* Inherited related functions

Helper classes are not part of the class documentation,
because grouping is made at the module level (see dedicated section above).

Methods are gathered in groups with predefined names and ordering (if applicable), as follows:

* Construction, i.e. destructor, constructors, assignment operators, `assign()` methods (`@group_construction`)
* Properties, e.g. `size()` and `reshape()` (`@group_properties`)
* Element access, e.g. `operator[]()` and `data()` (`@group_elements`)
* Iterators, e.g. `begin()` and `end()` (`@group_iterators`)
* Views, e.g. `slice()` and `entry()` (`@group_views`)
* Data modifiers, e.g. `apply()` and `operator+=()` (`@group_modifiers`)
* Operations, e.g. `find()` and `operator+()` (`@group_operations`)

Each method is documented as:

* Method declaration
* Brief description (`@brief`)
* Template parameters (`@tparam`)
* Parameters (`@param`)
* Detailed description (`@details`)
* Warnings (`@warning`)
* Template specializations (`@specialization`)
* See also (`@see`)
* Examples (`@par_example`)
* Notes (`@note`)

Some items can be skiped, for example
the parameters if names are explicit or standard enough,
or the template parameters if they should not be explicitely given by the user.


## Function documentation

Fuctions are documented similarly to methods,
with a few additional items:

* Parent module (`@ingroup`) **or** associated class (`@relates`)
* Function declaration
* Brief description (`@brief`)
* Template parameters (`@tparam`)
* Parameters (`@param`)
* Detailed description (`@details`)
* Warnings (`@warning`)
* Template specializations (`@specialization`)
* See also (`@see`)
* Examples (`@par_example`)
* Notes (`@note`)
