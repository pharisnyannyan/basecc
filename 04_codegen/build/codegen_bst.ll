; ModuleID = 'basecc'
source_filename = "basecc"

%struct.Node = type { i32, %struct.Node*, %struct.Node* }

@node_a = global %struct.Node zeroinitializer
@node_b = global %struct.Node zeroinitializer
@node_c = global %struct.Node zeroinitializer
@node_d = global %struct.Node zeroinitializer
@node_e = global %struct.Node zeroinitializer
@node_f = global %struct.Node zeroinitializer
@node_g = global %struct.Node zeroinitializer
define %struct.Node* @bst_find(%struct.Node* %root, i32 %value) {
entry:
  %t0 = alloca %struct.Node*
  store %struct.Node* null, %struct.Node** %t0
  %t1 = icmp eq %struct.Node* %root, null
  %t2 = zext i1 %t1 to i32
  %t3 = icmp ne i32 %t2, 0
  br i1 %t3, label %if.then0, label %if.end1
if.then0:
  ret %struct.Node* null
if.end1:
  %t4 = getelementptr inbounds %struct.Node, %struct.Node* %root, i32 0, i32 0
  %t5 = load i32, i32* %t4
  %t6 = sub i32 %t5, %value
  %t7 = icmp eq i32 %t6, 0
  %t8 = zext i1 %t7 to i32
  %t9 = icmp ne i32 %t8, 0
  br i1 %t9, label %if.then2, label %if.end3
if.then2:
  ret %struct.Node* %root
if.end3:
  %t10 = getelementptr inbounds %struct.Node, %struct.Node* %root, i32 0, i32 1
  %t11 = load %struct.Node*, %struct.Node** %t10
  %t12 = call %struct.Node* @bst_find(%struct.Node* %t11, i32 %value)
  store %struct.Node* %t12, %struct.Node** %t0
  %t13 = load %struct.Node*, %struct.Node** %t0
  %t14 = icmp ne %struct.Node* %t13, null
  br i1 %t14, label %if.then4, label %if.end5
if.then4:
  %t15 = load %struct.Node*, %struct.Node** %t0
  ret %struct.Node* %t15
if.end5:
  %t16 = getelementptr inbounds %struct.Node, %struct.Node* %root, i32 0, i32 2
  %t17 = load %struct.Node*, %struct.Node** %t16
  %t18 = call %struct.Node* @bst_find(%struct.Node* %t17, i32 %value)
  ret %struct.Node* %t18
}
define %struct.Node* @bst_build() {
entry:
  %t0 = alloca %struct.Node*
  store %struct.Node* null, %struct.Node** %t0
  %t1 = getelementptr inbounds %struct.Node, %struct.Node* @node_a, i32 0, i32 0
  store i32 5, i32* %t1
  %t2 = getelementptr inbounds %struct.Node, %struct.Node* @node_b, i32 0, i32 0
  store i32 3, i32* %t2
  %t3 = getelementptr inbounds %struct.Node, %struct.Node* @node_c, i32 0, i32 0
  store i32 8, i32* %t3
  %t4 = getelementptr inbounds %struct.Node, %struct.Node* @node_d, i32 0, i32 0
  store i32 1, i32* %t4
  %t5 = getelementptr inbounds %struct.Node, %struct.Node* @node_e, i32 0, i32 0
  store i32 4, i32* %t5
  %t6 = getelementptr inbounds %struct.Node, %struct.Node* @node_f, i32 0, i32 0
  store i32 7, i32* %t6
  %t7 = getelementptr inbounds %struct.Node, %struct.Node* @node_g, i32 0, i32 0
  store i32 9, i32* %t7
  %t8 = getelementptr inbounds %struct.Node, %struct.Node* @node_a, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t8
  %t9 = getelementptr inbounds %struct.Node, %struct.Node* @node_a, i32 0, i32 2
  store %struct.Node* null, %struct.Node** %t9
  %t10 = getelementptr inbounds %struct.Node, %struct.Node* @node_b, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t10
  %t11 = getelementptr inbounds %struct.Node, %struct.Node* @node_b, i32 0, i32 2
  store %struct.Node* null, %struct.Node** %t11
  %t12 = getelementptr inbounds %struct.Node, %struct.Node* @node_c, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t12
  %t13 = getelementptr inbounds %struct.Node, %struct.Node* @node_c, i32 0, i32 2
  store %struct.Node* null, %struct.Node** %t13
  %t14 = getelementptr inbounds %struct.Node, %struct.Node* @node_d, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t14
  %t15 = getelementptr inbounds %struct.Node, %struct.Node* @node_d, i32 0, i32 2
  store %struct.Node* null, %struct.Node** %t15
  %t16 = getelementptr inbounds %struct.Node, %struct.Node* @node_e, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t16
  %t17 = getelementptr inbounds %struct.Node, %struct.Node* @node_e, i32 0, i32 2
  store %struct.Node* null, %struct.Node** %t17
  %t18 = getelementptr inbounds %struct.Node, %struct.Node* @node_f, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t18
  %t19 = getelementptr inbounds %struct.Node, %struct.Node* @node_f, i32 0, i32 2
  store %struct.Node* null, %struct.Node** %t19
  %t20 = getelementptr inbounds %struct.Node, %struct.Node* @node_g, i32 0, i32 1
  store %struct.Node* null, %struct.Node** %t20
  %t21 = getelementptr inbounds %struct.Node, %struct.Node* @node_g, i32 0, i32 2
  store %struct.Node* null, %struct.Node** %t21
  store %struct.Node* @node_a, %struct.Node** %t0
  %t22 = getelementptr inbounds %struct.Node, %struct.Node* @node_a, i32 0, i32 1
  store %struct.Node* @node_b, %struct.Node** %t22
  %t23 = getelementptr inbounds %struct.Node, %struct.Node* @node_a, i32 0, i32 2
  store %struct.Node* @node_c, %struct.Node** %t23
  %t24 = getelementptr inbounds %struct.Node, %struct.Node* @node_b, i32 0, i32 1
  store %struct.Node* @node_d, %struct.Node** %t24
  %t25 = getelementptr inbounds %struct.Node, %struct.Node* @node_b, i32 0, i32 2
  store %struct.Node* @node_e, %struct.Node** %t25
  %t26 = getelementptr inbounds %struct.Node, %struct.Node* @node_c, i32 0, i32 1
  store %struct.Node* @node_f, %struct.Node** %t26
  %t27 = getelementptr inbounds %struct.Node, %struct.Node* @node_c, i32 0, i32 2
  store %struct.Node* @node_g, %struct.Node** %t27
  %t28 = load %struct.Node*, %struct.Node** %t0
  ret %struct.Node* %t28
}
define i32 @bst_search_sum() {
entry:
  %t0 = alloca %struct.Node*
  %t1 = call %struct.Node* @bst_build()
  store %struct.Node* %t1, %struct.Node** %t0
  %t2 = alloca i32
  store i32 0, i32* %t2
  %t3 = load %struct.Node*, %struct.Node** %t0
  %t4 = call %struct.Node* @bst_find(%struct.Node* %t3, i32 4)
  %t5 = icmp ne %struct.Node* %t4, null
  br i1 %t5, label %if.then0, label %if.end1
if.then0:
  %t6 = load i32, i32* %t2
  %t7 = add i32 %t6, 4
  store i32 %t7, i32* %t2
  br label %if.end1
if.end1:
  %t8 = load %struct.Node*, %struct.Node** %t0
  %t9 = call %struct.Node* @bst_find(%struct.Node* %t8, i32 6)
  %t10 = icmp ne %struct.Node* %t9, null
  br i1 %t10, label %if.then2, label %if.end3
if.then2:
  %t11 = load i32, i32* %t2
  %t12 = add i32 %t11, 100
  store i32 %t12, i32* %t2
  br label %if.end3
if.end3:
  %t13 = load %struct.Node*, %struct.Node** %t0
  %t14 = call %struct.Node* @bst_find(%struct.Node* %t13, i32 7)
  %t15 = icmp ne %struct.Node* %t14, null
  br i1 %t15, label %if.then4, label %if.end5
if.then4:
  %t16 = load i32, i32* %t2
  %t17 = add i32 %t16, 7
  store i32 %t17, i32* %t2
  br label %if.end5
if.end5:
  %t18 = load %struct.Node*, %struct.Node** %t0
  %t19 = call %struct.Node* @bst_find(%struct.Node* %t18, i32 9)
  %t20 = icmp ne %struct.Node* %t19, null
  br i1 %t20, label %if.then6, label %if.end7
if.then6:
  %t21 = load i32, i32* %t2
  %t22 = add i32 %t21, 9
  store i32 %t22, i32* %t2
  br label %if.end7
if.end7:
  %t23 = load i32, i32* %t2
  ret i32 %t23
}
