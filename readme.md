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
    
    支持的对象内部函数 :
    string.substr(offest);     截取字符串
    string.substr(offest,len); 截取字符串
    string.length();           获取字符串长度
    array.length();            获取数组长度

    支持的JavaScript 基本语句 :
    var var_name=expression;   变量声名
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

###漏洞利用部分

---

####1.UaF 原理部分(Use after Free ,重复使用已经被释放了的类)<br/><br/>
所有的HTML 元素在浏览器的内部都是一个类的实例.在`vuln_javascript` 中,所有关于HTML 元素的操作都在`javascript_element.cpp` 这个文件里面.img 和div 元素继承HTML 基础元素,同时HTML 基础元素类向下提供一些HTML 元素的通用函数实现方法(也就是`getAttribute()` ,`setAttribute()` 和`remove()`).<br/>
Uaf 的原理是:**当HTML 元素调用了remove() 删除自身并且在堆中释放内存之后,在接下来的代码执行流程中再次调用已经被释放的类时,将会引发释放后重用的漏洞(Use after Free)**<br/><br/>


####2.Read /Write Out of Bound 原理部分(越界读写)<br/><br/>
假设我们有两个数组:<br/><br/>
![buffer](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/buffer.png)<br/><br/>
在真实的环境中,这两个数组是有可能是相连的<br/><br/>
![buffer_in_memory](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/buffer_in_memory.png)<br/><br/>
通常情况下,我们都可以正常访问buffer1 里面的数据,假设往buffer1 里面读写数据的时候的时候一不小心就越过了buffer1 本来的长度到了buffer2 呢?<br/><br/>
![read_write_out_of_buffer1](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_write_out_of_buffer1.png)<br/><br/>

Exapmle 1 -- String 对象substr() 读取越界:<br/><br/>
**Exploit** :<br/>
```javascript
var first_string='test string';
var read_string='read me ...';

var read_string_length=first_string.substr(0x28,4);
var read_string_data=first_string.substr(0x30,read_string.length());
console.log('read_string_length:'+read_string_length);
console.log('read_string_data:'+read_string_length);
```
当用户声名一个String 类型的变量时,JavaScript 会申请堆内存来保存String 对象的数据<br/>
```javascript
var string='test string';
console.log(string);
```
![base_string_in_heap](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/base_string_in_heap.png)<br/><br/>
根据结构可知:0x0CF1D870 中保存的为字符串长度,0x0CF1D874 为字符串的保存地址,0x0CF1D878 为字符串的数据地址<br/><br/>
![base_string_in_heap_detail](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/base_string_in_heap_detail.png)<br/><br/>
substr() 越界读取示例:<br/>
```javascript
var string='test string';
var read_out_of_bound=string.substr(string.length(),10);
console.log(read_out_of_bound);
```
运行情况:<br/><br/>
![read_out_of_bound_execute](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_execute.png)<br/><br/>
接下来我们到`javascript_function.cpp string_object_substr()` 里面设置断点,执行程序解析脚本,观测数据复制情况<br/><br/>
![read_out_of_bound_memcpy](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_memcpy.png)<br/><br/>
可以看到,`substr()` 的`offset` 参数的设置使得`memcpy()` 直接读取到string 对象的NULL 结束符的位置,然后到`javascript_function.cpp console_log()` 中设置断点,可以看到即将要输出到控制台的read_out_of_bound 对象的内容<br/><br/>
![read_out_of_bound_output](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_output.png)<br/><br/>
那么我们可以构造两个String 对象,让第一个String 越界读取到第二个String 对象里面,当我们声名了两个String 对象时,堆里面的内容如下<br/>
声名两个String 对象示例代码:
```javascript
var first_string='test string';
var read_string='read me ...';
console.log('exit');
```
![two_string_in_heap](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/two_string_in_heap.png)<br/><br/>
由于这两个字符串在堆里面是连续的,那么我们可以构造好`first_string.substr()` 的调用参数读取到`read_string` 里面的内容,首先分析一下substr() 的参数应该如何构造<br/>
first_string 越界读写测试代码一:
```javascript
var first_string='test string';
var read_string='read me ...';
var read_data=first_string.substr(0,4);
console.log(read_data);
```
现在到回到`javascript_function.cpp string_object_substr()` 里面设置断点,可以看到`memcpy()` 将要从`first_string` 保存数据的地址中开始复制数据<br/>
![read_out_of_bound_memcpy_exploit](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_memcpy_exploit.png)<br/><br/>
现在,可以这样计算,`read_string` 保存数据的地址为0xCEFD848 ,`first_string` 保存数据的地址为0xCEFD818 ,地址偏移了0x30 ,于是可以构造`first_string.substr(0x30,4);` 读取到`read_string` 里面的内容,继续观察`javascript_function.cpp string_object_substr()` 的执行情况<br/><br/>
first_string 越界读写测试代码二:
```javascript
var first_string='test string';
var read_string='read me ...';
var read_data=first_string.substr(0x30,4);
console.log(read_data);
```
![read_out_of_bound_memcpy_read_data](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_memcpy_read_data.png)<br/><br/>
可以看到,我们已经控制`substr()` 读取到了`read_string` 的内容了<br/><br/>
![read_out_of_bound_memcpy_read_data_output](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_memcpy_read_data_output.png)<br/><br/>
由于我们读取的是4 字节的数据,要想完全读取`read_data` 的内容,只需要把`first_string.substr(0x30,4)` 修改为`first_string.substr(0x30,read_string.length())` 即可<br/><br/>
但是只读到内容并没有什么用途,根据之前的分析,我们可以跨过去读取一个对象的信息,比如这样:<br/>
substr() 越界读取object 虚函数表:
```javascript
var first_string='test string';
var read_object=document.createElement('img');
var read_data=first_string.substr(0x50,4);
console.log(read_data);
```
现在,请到`javascript_element.cpp base_element::base_element()` 中设置断点,中断时观察堆里面的数据,可以看到`first_string` 和`read_object` 对象的地址偏移动从原来的0x30 变成了0x50 (`0xD07D868-0xD07D810=0x50`)<br/><br/>
![read_out_of_bound_object](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_object.png)<br/><br/>
然后再到`javascript_function.cpp string_object_substr()` 中观察`memcpy()` ,发现虚函数表已经复制到变量里面中去了<br/><br/>
![read_out_of_bound_read_object_virtual_table](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_read_object_virtual_table.png)<br/><br/>
因为`substr()` 是以String 对象读取出来的,所以会输出的时候会显示错误<br/><br/>
![read_out_of_bound_read_object_output](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_read_object_output.png)<br/><br/>
TIPS! 所有的测试都在`Debug` 选项下的`Debug` 模式下进行..
