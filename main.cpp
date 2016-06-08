
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
    for (1;2;3) {code_block};
    if {code_block} [else if {code_line;] [else {code_block}]

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

void main(void) {
    init_javascript_envirment();/*
    eval("var a=1;");
    eval("var b=3");
    eval("var c='AAAAAA';");
    eval("var d=c;");
    eval("e=a;");
    eval("console.log(a);");
    eval("console.log(c);");
    eval("console.log(d);");
    eval("console.log(e);");
    eval("console.log(2-1);");
    eval("console.log(1*2);");
    eval("console.log(4/1);");
    eval("console.log(a+c)");
    eval("console.log(c+c)");*/

//    eval("var a=1+(2+3)+4+((1+1+2)+3);"); <- WARNING! ..
/*
    */
    eval("var a=document.createElement('img');");
    eval("var b=document.createElement('img');");
    eval("var c=document.createElement('img');");
    eval("var d=document.createElement('img');");
    eval("a.setAttribute('123','AAA');");
    eval("console.log(a.getAttribute('123'));a.remove();a.getAttribute('123');");
                                                                                                    /*
    eval("var b=1;");
    eval("a=b+1;");
    eval("console.log(a);");
    eval("a=a+1;");
    eval("console.log(a);");
    eval("a+=1;");
    eval("console.log(a);");
    eval("for (var c=1;c<10;c+=1) {a+=1;}");
    eval("console.log(a);");*/
    //eval("{var a=1;{var b=2;}console.log(a+b);}");
    /*
    eval("console.log(1==1);");
    eval("console.log(1>=1);");
    eval("console.log(1==2);");
    eval("console.log(1<3);");*/
}

/*
void main(void) {
    init_envirment();
    /*
    set_variant("string1",(void*)"1234",STRING);
    set_variant("string2",(void*)"just test!",STRING);
    copy_variant("string_copy","string2");
    long output_data=0;
    support_javascript_variant_type output_type=NONE;
    get_variant("string1",(void*)&output_data,&output_type);
    printf("string1=%s\n",output_data);
    get_variant("string2",(void*)&output_data,&output_type);
    printf("string2=%s\n",output_data);
    get_variant("string_copy",(void*)&output_data,&output_type);
    printf("string_copy=%s\n",output_data);*/
    /*
    string calcu("1+(2+(1+1))");
    if (execute_calculation_express(calcu)) {
        long return_data=0;
        support_javascript_variant_type return_type=NONE;
        get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&return_data,&return_type);
        printf("Calcu=%d",return_data);
    } else
        printf("ERR");
        */
    /*
    string a("0x100");
    printf("%d",hex_string_to_number(a));*/
    /*
    string express("(()()");
    printf("%d\n",get_matching_outside_right_bracket(express,0));
    express="()()";
    printf("%d\n",get_matching_outside_right_bracket(express,0));
    express="(()())";
    printf("%d\n",get_matching_outside_right_bracket(express,0));
    express=")";
    printf("%d\n",get_matching_outside_right_bracket(express,0));
}
*/
