###vuln_javascript
![vuln_javascript](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/vuln_javascript_logo.jpg)

---

`vuln_javascript` 模拟真实的浏览器的JavaScript 环境,通常地,我们使用JavaScript 来精心设计一些操作DOM 和其它浏览器对象的逻辑代码时会使得浏览器产生崩溃,针对不同种类的崩溃有不同的利用方法.`vuln_javascript` 收集了UAF 和越界读写两种漏洞类型,希望`vuln_javascript` 可以帮助更多安全爱好者理解浏览器内核漏洞原理和细节还有相关的Exploit 编写技巧.由于`vuln_javascript` 并不是真正的JavaScript 执行环境,所以现在只支持执行JavaScript 语句很少,可执行的语句如下:<br/><br/>

    支持的document 对象 :
    document.createElement();  创建HTML 元素
    document.appendChild();    附加到页面上显示

    可以创建的HTML 元素:
    img                        img 图片元素
    div                        div 容器元素

    支持的元素内部函数 :
    element.remove();          删除元素
    element.getAttribute();    获取属性
    element.setAttribute();    设置属性

    支持的JavaScript 基本语句 :
    var var_name=expression;   变量声名
    var_name.substr();         调用元素内部函数
    for (1;2;3) {code_block};  for 循环语句
    if {code_block} [else if {code_line;] [else {code_block}]            if 判断语句
    function function_name(function_argment_list) {function_code_block}  函数声名语句

    支持的表达式计算 :
    + - * / ()                 四则运算
    ''                         使用'' 号来表示字符串
    == != > >= ...             表达式判断
    call();                    函数调用
    new IntArray();            生成数字数组
    new IntArray(length);      生成长度为length 的数字数组
    new IntArray(num1,num2);   生成长度为2 且包含内容为num1,num2 的数字数组
    new ObjArray();            生成对象数组
    new ObjArray(length);      生成长度为length 的对象数组

    支持的元素方法 :
    var_name.substr(l,b);      元素函数调用
    var_name[1];               设置数组索引

    其它的JavaScript 函数 :
    console.log();             输出数据

###vuln_javacript 使用

---

`vuln_javacript.exe` 编译完成的EXE 在`/Release` 路径下可以找到(编译IDE 环境:VC++ 6 ,没有导入任何其它库).<br/><br/>
`vuln_javacript.exe` 可以选择带参数运行,指定的参数为即将要执行的JavaScript 代码文件路径,例子:<br/><br/>
![buffer_in_memory](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/example_using_run_file.png)<br/><br/>
`vuln_javacript.exe` 也可以不带参数运行,默认以控制台的形式执行代码,例子:<br/><br/>
![buffer_in_memory](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/example_using_console_mode.png)<br/><br/>
退出控制台模式的命令为`quit` ..<br/><br/>
**WARNING!** 有个关于Array 使用的Bug (应用程序崩溃),因为VC++ 6 本身的STL 库设计问题,以后会移植到VC++ 8 ..

###vuln_javascript 执行例子

---

Example 1 -- 简单的计算: <br/>

    var num_1;
    var num_2=123;
    num_1=321;
    console.log(num_1+num_2);

Example 2 -- 简单的if 判断: <br/>

    var array=new IntArray(10);
    if (array.length())
        console.log('alloc success');
    else
	   console.log('alloc falut');

Example 3 -- 简单的函数调用: <br/>

    function output(info) {
        console.log(info);
    }
        
    function calcu() {
	   var a=1;
	   for (var index=1;index<=10;index+=1)
            a+=index;
	   return a;
    }

    output(calcu());
    console.log('exit!..');

###漏洞利部分

---

1.UaF 原理部分(Use after Free ,重复使用已经被释放了的类)<br/><br/>
所有的HTML 元素在浏览器的内部都是一个类的实例.在`vuln_javascript` 中,所有关于HTML 元素的操作都在`javascript_element.cpp` 这个文件里面.img 和div 元素继承HTML 基础元素,同时HTML 基础元素类向下提供一些HTML 元素的通用函数实现方法(也就是`getAttribute()` ,`setAttribute()` 和`remove()`).<br/>
Uaf 的原理是:**当HTML 元素调用了remove() 删除自身并且在堆中释放内存之后,在接下来的代码执行流程中再次调用已经被释放的类时,将会引发释放后重用的漏洞(Use after Free)**<br/>
2.Read /Write Out of Bound 原理部分(越界读写)<br/><br/>
假设我们有两个数组:<br/>
![buffer](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/buffer.png)<br/>
在真实的环境中,这两个数组是有可能是相连的<br/>
![buffer_in_memory](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/buffer_in_memory.png)<br/>
通常情况下,我们都可以正常访问buffer1 里面的数据,假设往buffer1 里面读写数据的时候的时候一不小心就越过了buffer1 本来的长度到了buffer2 呢?<br/>
![read_write_out_of_buffer1](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_write_out_of_buffer1.png)<br/>
