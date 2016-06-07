
#include <stdio.h>

#include "javascript_envirment.h"

/*
    UaF and memory out of bound ..

    Support Document Function :
    document.createElement();     -> HeapAlloc
    document.appendChild();       -> None

    Support Element :
    img
    div

    Support Element Function :
    element.remove();             -> HeapFree
    element.innerHTML;            -> Use Element
    element.setAttribute()        -> Use Element

    Support JavaScript :
    var var_name=expression;      -> var var_name=eval(expression)
    var_name.substr();

    Support Expression :
    + - * /
    call();
    new Array();                  -> HeapAlloc

    Support Var Function :
    var_name.substr(l,b);
    var_name[1];
    var_name.attribute=?????;

    Other Function :
    console.log();
*/
