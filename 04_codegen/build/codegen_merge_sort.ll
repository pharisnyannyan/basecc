; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @is_leq(i32 %left, i32 %right) {
entry:
  %t0 = icmp ne i32 %right, 0
  br i1 %t0, label %if.then0, label %if.end1
if.then0:
  ret i32 1
if.end1:
  %t1 = icmp ne i32 %left, 0
  br i1 %t1, label %if.then2, label %if.end3
if.then2:
  ret i32 0
if.end3:
  ret i32 1
}
define i32 @merge_into(i32* %values, i32* %temp, i32 %low, i32 %mid, i32 %high) {
entry:
  %t0 = alloca i32
  store i32 %low, i32* %t0
  %t1 = alloca i32
  %t2 = add i32 %mid, 1
  store i32 %t2, i32* %t1
  %t3 = alloca i32
  store i32 %low, i32* %t3
  %t4 = alloca i32
  %t5 = load i32, i32* %t0
  %t6 = sub i32 %mid, %t5
  %t7 = add i32 %t6, 1
  store i32 %t7, i32* %t4
  %t8 = alloca i32
  %t9 = load i32, i32* %t1
  %t10 = sub i32 %high, %t9
  %t11 = add i32 %t10, 1
  store i32 %t11, i32* %t8
  br label %while.cond0
while.cond0:
  br label %logic.left3
logic.left3:
  %t12 = load i32, i32* %t4
  %t13 = icmp ne i32 %t12, 0
  br i1 %t13, label %logic.rhs4, label %logic.end5
logic.rhs4:
  %t14 = load i32, i32* %t8
  %t15 = icmp ne i32 %t14, 0
  br label %logic.end5
logic.end5:
  %t16 = phi i1 [0, %logic.left3], [%t15, %logic.rhs4]
  %t17 = zext i1 %t16 to i32
  %t18 = icmp ne i32 %t17, 0
  br i1 %t18, label %while.body1, label %while.end2
while.body1:
  %t19 = load i32, i32* %t0
  %t20 = getelementptr i32, i32* %values, i32 %t19
  %t21 = load i32, i32* %t20
  %t22 = load i32, i32* %t1
  %t23 = getelementptr i32, i32* %values, i32 %t22
  %t24 = load i32, i32* %t23
  %t25 = call i32 @is_leq(i32 %t21, i32 %t24)
  %t26 = icmp ne i32 %t25, 0
  br i1 %t26, label %if.then6, label %if.else7
if.then6:
  %t27 = load i32, i32* %t3
  %t28 = getelementptr i32, i32* %temp, i32 %t27
  %t29 = load i32, i32* %t0
  %t30 = getelementptr i32, i32* %values, i32 %t29
  %t31 = load i32, i32* %t30
  store i32 %t31, i32* %t28
  %t32 = load i32, i32* %t0
  %t33 = add i32 %t32, 1
  store i32 %t33, i32* %t0
  br label %if.end8
if.else7:
  %t34 = load i32, i32* %t3
  %t35 = getelementptr i32, i32* %temp, i32 %t34
  %t36 = load i32, i32* %t1
  %t37 = getelementptr i32, i32* %values, i32 %t36
  %t38 = load i32, i32* %t37
  store i32 %t38, i32* %t35
  %t39 = load i32, i32* %t1
  %t40 = add i32 %t39, 1
  store i32 %t40, i32* %t1
  br label %if.end8
if.end8:
  %t41 = load i32, i32* %t3
  %t42 = add i32 %t41, 1
  store i32 %t42, i32* %t3
  %t43 = load i32, i32* %t0
  %t44 = sub i32 %mid, %t43
  %t45 = add i32 %t44, 1
  store i32 %t45, i32* %t4
  %t46 = load i32, i32* %t1
  %t47 = sub i32 %high, %t46
  %t48 = add i32 %t47, 1
  store i32 %t48, i32* %t8
  br label %while.cond0
while.end2:
  %t49 = load i32, i32* %t0
  %t50 = sub i32 %mid, %t49
  %t51 = add i32 %t50, 1
  store i32 %t51, i32* %t4
  br label %while.cond9
while.cond9:
  %t52 = load i32, i32* %t4
  %t53 = icmp ne i32 %t52, 0
  br i1 %t53, label %while.body10, label %while.end11
while.body10:
  %t54 = load i32, i32* %t3
  %t55 = getelementptr i32, i32* %temp, i32 %t54
  %t56 = load i32, i32* %t0
  %t57 = getelementptr i32, i32* %values, i32 %t56
  %t58 = load i32, i32* %t57
  store i32 %t58, i32* %t55
  %t59 = load i32, i32* %t0
  %t60 = add i32 %t59, 1
  store i32 %t60, i32* %t0
  %t61 = load i32, i32* %t3
  %t62 = add i32 %t61, 1
  store i32 %t62, i32* %t3
  %t63 = load i32, i32* %t0
  %t64 = sub i32 %mid, %t63
  %t65 = add i32 %t64, 1
  store i32 %t65, i32* %t4
  br label %while.cond9
while.end11:
  %t66 = load i32, i32* %t1
  %t67 = sub i32 %high, %t66
  %t68 = add i32 %t67, 1
  store i32 %t68, i32* %t8
  br label %while.cond12
while.cond12:
  %t69 = load i32, i32* %t8
  %t70 = icmp ne i32 %t69, 0
  br i1 %t70, label %while.body13, label %while.end14
while.body13:
  %t71 = load i32, i32* %t3
  %t72 = getelementptr i32, i32* %temp, i32 %t71
  %t73 = load i32, i32* %t1
  %t74 = getelementptr i32, i32* %values, i32 %t73
  %t75 = load i32, i32* %t74
  store i32 %t75, i32* %t72
  %t76 = load i32, i32* %t1
  %t77 = add i32 %t76, 1
  store i32 %t77, i32* %t1
  %t78 = load i32, i32* %t3
  %t79 = add i32 %t78, 1
  store i32 %t79, i32* %t3
  %t80 = load i32, i32* %t1
  %t81 = sub i32 %high, %t80
  %t82 = add i32 %t81, 1
  store i32 %t82, i32* %t8
  br label %while.cond12
while.end14:
  %t83 = alloca i32
  %t84 = sub i32 %high, %low
  %t85 = add i32 %t84, 1
  store i32 %t85, i32* %t83
  %t86 = alloca i32
  store i32 0, i32* %t86
  br label %for.cond15
for.cond15:
  %t87 = load i32, i32* %t83
  %t88 = load i32, i32* %t86
  %t89 = sub i32 %t87, %t88
  %t90 = icmp ne i32 %t89, 0
  br i1 %t90, label %for.body16, label %for.end18
for.body16:
  %t91 = getelementptr i32, i32* %values, i32 %low
  %t92 = load i32, i32* %t86
  %t93 = getelementptr i32, i32* %t91, i32 %t92
  %t94 = getelementptr i32, i32* %temp, i32 %low
  %t95 = load i32, i32* %t86
  %t96 = getelementptr i32, i32* %t94, i32 %t95
  %t97 = load i32, i32* %t96
  store i32 %t97, i32* %t93
  br label %for.inc17
for.inc17:
  %t98 = load i32, i32* %t86
  %t99 = add i32 %t98, 1
  store i32 %t99, i32* %t86
  br label %for.cond15
for.end18:
  ret i32 0
}
define i32 @merge_sort(i32* %values, i32* %temp, i32 %low, i32 %high) {
entry:
  %t0 = alloca i32
  store i32 0, i32* %t0
  %t1 = alloca i32
  store i32 0, i32* %t1
  %t2 = sub i32 %high, %low
  %t3 = icmp eq i32 %t2, 0
  %t4 = zext i1 %t3 to i32
  %t5 = icmp ne i32 %t4, 0
  br i1 %t5, label %if.then0, label %if.end1
if.then0:
  ret i32 0
if.end1:
  %t6 = sub i32 %high, %low
  %t7 = sdiv i32 %t6, 2
  %t8 = add i32 %low, %t7
  store i32 %t8, i32* %t0
  %t9 = load i32, i32* %t0
  %t10 = call i32 @merge_sort(i32* %values, i32* %temp, i32 %low, i32 %t9)
  store i32 %t10, i32* %t1
  %t11 = load i32, i32* %t0
  %t12 = add i32 %t11, 1
  %t13 = call i32 @merge_sort(i32* %values, i32* %temp, i32 %t12, i32 %high)
  store i32 %t13, i32* %t1
  %t14 = load i32, i32* %t0
  %t15 = call i32 @merge_into(i32* %values, i32* %temp, i32 %low, i32 %t14, i32 %high)
  store i32 %t15, i32* %t1
  ret i32 0
}
