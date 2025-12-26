; ModuleID = 'basecc'
source_filename = "basecc"

%struct.Node = type { i32, %struct.Node* }

@node_a = global %struct.Node zeroinitializer
@node_b = global %struct.Node zeroinitializer
@node_c = global %struct.Node zeroinitializer
@node_d = global %struct.Node zeroinitializer
@node_e = global %struct.Node zeroinitializer
define %struct.Node* @list_init() {
entry:
  %t0 = alloca %struct.Node*
  store %struct.Node* @node_a, %struct.Node** %t0
  %t1 = alloca %struct.Node*
  store %struct.Node* @node_a, %struct.Node** %t1
  %t2 = load %struct.Node*, %struct.Node** %t1
  %t3 = getelementptr inbounds %struct.Node, %struct.Node* %t2, i32 0, i32 0
  store i32 1, i32* %t3
  %t4 = load %struct.Node*, %struct.Node** %t1
  %t5 = getelementptr inbounds %struct.Node, %struct.Node* %t4, i32 0, i32 1
  store %struct.Node* @node_b, %struct.Node** %t5
  store %struct.Node* @node_b, %struct.Node** %t1
  %t6 = load %struct.Node*, %struct.Node** %t1
  %t7 = getelementptr inbounds %struct.Node, %struct.Node* %t6, i32 0, i32 0
  store i32 2, i32* %t7
  %t8 = load %struct.Node*, %struct.Node** %t1
  %t9 = getelementptr inbounds %struct.Node, %struct.Node* %t8, i32 0, i32 1
  store %struct.Node* @node_c, %struct.Node** %t9
  store %struct.Node* @node_c, %struct.Node** %t1
  %t10 = load %struct.Node*, %struct.Node** %t1
  %t11 = getelementptr inbounds %struct.Node, %struct.Node* %t10, i32 0, i32 0
  store i32 3, i32* %t11
  %t12 = load %struct.Node*, %struct.Node** %t1
  %t13 = getelementptr inbounds %struct.Node, %struct.Node* %t12, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t13
  store %struct.Node* @node_d, %struct.Node** %t1
  %t14 = load %struct.Node*, %struct.Node** %t1
  %t15 = getelementptr inbounds %struct.Node, %struct.Node* %t14, i32 0, i32 0
  store i32 4, i32* %t15
  %t16 = load %struct.Node*, %struct.Node** %t1
  %t17 = getelementptr inbounds %struct.Node, %struct.Node* %t16, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t17
  store %struct.Node* @node_e, %struct.Node** %t1
  %t18 = load %struct.Node*, %struct.Node** %t1
  %t19 = getelementptr inbounds %struct.Node, %struct.Node* %t18, i32 0, i32 0
  store i32 5, i32* %t19
  %t20 = load %struct.Node*, %struct.Node** %t1
  %t21 = getelementptr inbounds %struct.Node, %struct.Node* %t20, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t21
  %t22 = load %struct.Node*, %struct.Node** %t0
  ret %struct.Node* %t22
}
define %struct.Node* @list_push_front(%struct.Node* %head, %struct.Node* %node) {
entry:
  %t0 = getelementptr inbounds %struct.Node, %struct.Node* %node, i32 0, i32 1
  store %struct.Node* %head, %struct.Node** %t0
  ret %struct.Node* %node
}
define %struct.Node* @list_insert_after(%struct.Node* %pos, %struct.Node* %node) {
entry:
  %t0 = getelementptr inbounds %struct.Node, %struct.Node* %node, i32 0, i32 1
  %t1 = getelementptr inbounds %struct.Node, %struct.Node* %pos, i32 0, i32 1
  %t2 = load %struct.Node*, %struct.Node** %t1
  store %struct.Node* %t2, %struct.Node** %t0
  %t3 = getelementptr inbounds %struct.Node, %struct.Node* %pos, i32 0, i32 1
  store %struct.Node* %node, %struct.Node** %t3
  ret %struct.Node* %node
}
define %struct.Node* @list_remove_after(%struct.Node* %pos) {
entry:
  %t0 = alloca %struct.Node*
  %t1 = getelementptr inbounds %struct.Node, %struct.Node* %pos, i32 0, i32 1
  %t2 = load %struct.Node*, %struct.Node** %t1
  store %struct.Node* %t2, %struct.Node** %t0
  %t3 = load %struct.Node*, %struct.Node** %t0
  %t4 = icmp eq %struct.Node* %t3, null
  %t5 = zext i1 %t4 to i32
  %t6 = icmp ne i32 %t5, 0
  br i1 %t6, label %if.then0, label %if.end1
if.then0:
  ret %struct.Node* null
if.end1:
  %t7 = getelementptr inbounds %struct.Node, %struct.Node* %pos, i32 0, i32 1
  %t8 = load %struct.Node*, %struct.Node** %t0
  %t9 = getelementptr inbounds %struct.Node, %struct.Node* %t8, i32 0, i32 1
  %t10 = load %struct.Node*, %struct.Node** %t9
  store %struct.Node* %t10, %struct.Node** %t7
  %t11 = load %struct.Node*, %struct.Node** %t0
  %t12 = getelementptr inbounds %struct.Node, %struct.Node* %t11, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t12
  %t13 = load %struct.Node*, %struct.Node** %t0
  ret %struct.Node* %t13
}
define %struct.Node* @list_find(%struct.Node* %head, i32 %value) {
entry:
  %t0 = alloca %struct.Node*
  store %struct.Node* %head, %struct.Node** %t0
  br label %while.cond0
while.cond0:
  %t1 = load %struct.Node*, %struct.Node** %t0
  %t2 = icmp ne %struct.Node* %t1, null
  br i1 %t2, label %while.body1, label %while.end2
while.body1:
  %t3 = alloca i32
  %t4 = load %struct.Node*, %struct.Node** %t0
  %t5 = getelementptr inbounds %struct.Node, %struct.Node* %t4, i32 0, i32 0
  %t6 = load i32, i32* %t5
  %t7 = sub i32 %t6, %value
  store i32 %t7, i32* %t3
  %t8 = load i32, i32* %t3
  %t9 = icmp eq i32 %t8, 0
  %t10 = zext i1 %t9 to i32
  %t11 = icmp ne i32 %t10, 0
  br i1 %t11, label %if.then3, label %if.end4
if.then3:
  %t12 = load %struct.Node*, %struct.Node** %t0
  ret %struct.Node* %t12
if.end4:
  %t13 = load %struct.Node*, %struct.Node** %t0
  %t14 = getelementptr inbounds %struct.Node, %struct.Node* %t13, i32 0, i32 1
  %t15 = load %struct.Node*, %struct.Node** %t14
  store %struct.Node* %t15, %struct.Node** %t0
  br label %while.cond0
while.end2:
  ret %struct.Node* null
}
define i32 @list_smoke() {
entry:
  %t0 = alloca %struct.Node*
  %t1 = call %struct.Node* @list_init()
  store %struct.Node* %t1, %struct.Node** %t0
  %t2 = alloca %struct.Node*
  store %struct.Node* null, %struct.Node** %t2
  %t3 = alloca %struct.Node*
  store %struct.Node* null, %struct.Node** %t3
  %t4 = alloca %struct.Node*
  store %struct.Node* null, %struct.Node** %t4
  %t5 = alloca i32
  store i32 0, i32* %t5
  %t6 = load %struct.Node*, %struct.Node** %t0
  %t7 = call %struct.Node* @list_push_front(%struct.Node* %t6, %struct.Node* @node_e)
  store %struct.Node* %t7, %struct.Node** %t0
  %t8 = load %struct.Node*, %struct.Node** %t0
  %t9 = call %struct.Node* @list_insert_after(%struct.Node* %t8, %struct.Node* @node_d)
  store %struct.Node* %t9, %struct.Node** %t2
  %t10 = load %struct.Node*, %struct.Node** %t0
  %t11 = call %struct.Node* @list_remove_after(%struct.Node* %t10)
  store %struct.Node* %t11, %struct.Node** %t3
  %t12 = load %struct.Node*, %struct.Node** %t0
  %t13 = call %struct.Node* @list_find(%struct.Node* %t12, i32 2)
  store %struct.Node* %t13, %struct.Node** %t4
  %t14 = load i32, i32* %t5
  %t15 = load %struct.Node*, %struct.Node** %t0
  %t16 = getelementptr inbounds %struct.Node, %struct.Node* %t15, i32 0, i32 0
  %t17 = load i32, i32* %t16
  %t18 = add i32 %t14, %t17
  store i32 %t18, i32* %t5
  %t19 = load %struct.Node*, %struct.Node** %t3
  %t20 = icmp ne %struct.Node* %t19, null
  br i1 %t20, label %if.then0, label %if.end1
if.then0:
  %t21 = load i32, i32* %t5
  %t22 = load %struct.Node*, %struct.Node** %t3
  %t23 = getelementptr inbounds %struct.Node, %struct.Node* %t22, i32 0, i32 0
  %t24 = load i32, i32* %t23
  %t25 = add i32 %t21, %t24
  store i32 %t25, i32* %t5
  br label %if.end1
if.end1:
  %t26 = load %struct.Node*, %struct.Node** %t4
  %t27 = icmp ne %struct.Node* %t26, null
  br i1 %t27, label %if.then2, label %if.end3
if.then2:
  %t28 = load i32, i32* %t5
  %t29 = load %struct.Node*, %struct.Node** %t4
  %t30 = getelementptr inbounds %struct.Node, %struct.Node* %t29, i32 0, i32 0
  %t31 = load i32, i32* %t30
  %t32 = add i32 %t28, %t31
  store i32 %t32, i32* %t5
  br label %if.end3
if.end3:
  %t33 = load i32, i32* %t5
  ret i32 %t33
}
