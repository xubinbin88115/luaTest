#include <stdio.h>
#include <setjmp.h>

// int setjmp(jmp_buf environment)
// 这个宏可能不只返回一次。第一次，在直接调用它时，它总是返回零。当调用 longjmp 时带有设置的环境信息，这个宏会再次返回，此时它返回的值会传给 longjmp 作为第二个参数。

static jmp_buf buf;
 
void second(void) {
    printf("second\n");         // 打印
    longjmp(buf,1);             // 跳回setjmp的调用处 - 使得setjmp返回值为1
}
 
void first(void) {
    second();
    printf("first\n");          // 不可能执行到此行
}
 
int main() {   
    if ( ! setjmp(buf) ) {
        first();                // 进入此行前，setjmp返回0
    } else {                    // 当longjmp跳转回，setjmp返回1，因此进入此行
        printf("main\n");       // 打印
    }
 
    return 0;
}