%struct._IO_FILE = type { i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i32, i64, i16, i8, [1 x i8], ptr, i64, ptr, ptr, ptr, ptr, i64, i32, [20 x i8] }
%struct.anon.2 = type { i32, i32 }
%struct.anon.3 = type { i64, i64 }
%struct.anon.4 = type { i64, i64 }

@g = internal global i32 0, align 4
@stderr = external global ptr
@stdout = external global ptr
@stdin = external global ptr

define internal void @side_effect() #0 {
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_expression_statements() #0 {
	%2 = alloca i32, align 4
	store i32 42, ptr %2, align 4
	%3 = load i32, ptr %2, align 4
	%4 = add nsw i32 %3, 1
	store i32 %4, ptr %2, align 4
	%5 = load i32, ptr %2, align 4
	%6 = mul nsw i32 %5, 2
	store i32 %6, ptr %2, align 4
	%7 = load i32, ptr %2, align 4
	%8 = sdiv i32 %7, 2
	store i32 %8, ptr %2, align 4
	%9 = load i32, ptr %2, align 4
	%10 = srem i32 %9, 7
	store i32 %10, ptr %2, align 4
	%11 = load i32, ptr %2, align 4
	%12 = and i32 %11, 0
	store i32 %12, ptr %2, align 4
	%13 = load i32, ptr %2, align 4
	%14 = or i32 %13, 0
	store i32 %14, ptr %2, align 4
	%15 = load i32, ptr %2, align 4
	%16 = xor i32 %15, 0
	store i32 %16, ptr %2, align 4
	%17 = load i32, ptr %2, align 4
	%18 = shl i32 %17, 2
	store i32 %18, ptr %2, align 4
	%19 = load i32, ptr %2, align 4
	%20 = ashr i32 %19, 1
	store i32 %20, ptr %2, align 4
	call void @side_effect()
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_compound_statements() #0 {
	%2 = alloca i32, align 4
	store i32 1, ptr %2, align 4
	%3 = alloca i32, align 4
	store i32 2, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 3, ptr %4, align 4
	%5 = alloca i32, align 4
	store i32 0, ptr %5, align 4
	%6 = alloca i32, align 4
	%7 = load i32, ptr %5, align 4
	%8 = mul nsw i32 %7, 2
	store i32 %8, ptr %6, align 4
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_selection_statements(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = alloca double, align 8
	store double 0.000000e+00, ptr %4, align 8
	%7 = load i32, ptr %2, align 4
	%8 = icmp sgt i32 %7, 0
	%9 = icmp ne i1 %8, 0
	br i1 %9, label %label_6, label %label_5

label_6:
	%10 = load double, ptr %4, align 8
	%11 = fadd double %10, 1.000000
	store double %11, ptr %4, align 8
	br label %label_5

label_5:
	%15 = load i32, ptr %2, align 4
	%16 = icmp sge i32 %15, 0
	%17 = icmp ne i1 %16, 0
	br i1 %17, label %label_13, label %label_14

label_13:
	%18 = load double, ptr %4, align 8
	%19 = fadd double %18, 2.000000
	store double %19, ptr %4, align 8
	br label %label_12

label_14:
	%20 = load double, ptr %4, align 8
	%21 = fadd double %20, 3.000000
	store double %21, ptr %4, align 8
	br label %label_12

label_12:
	%22 = alloca i32, align 4
	store i32 11, ptr %22, align 4
	%26 = load i32, ptr %2, align 4
	%27 = icmp slt i32 %26, 0
	%28 = icmp ne i1 %27, 0
	br i1 %28, label %label_24, label %label_25

label_24:
	br label %label_23

label_25:
	%31 = load i32, ptr %2, align 4
	%32 = icmp eq i32 %31, 0
	%33 = icmp ne i1 %32, 0
	br i1 %33, label %label_29, label %label_30

label_29:
	%34 = load i32, ptr %22, align 4
	%35 = mul nsw i32 %34, 32
	store i32 %35, ptr %22, align 4
	br label %label_23

label_30:
	%36 = load i32, ptr %22, align 4
	%37 = sdiv i32 %36, 2
	store i32 %37, ptr %22, align 4
	br label %label_23

label_23:
	%40 = load i32, ptr %2, align 4
	%41 = icmp sle i32 %40, 0
	%42 = icmp ne i1 %41, 0
	br i1 %42, label %label_39, label %label_38

label_39:
	%46 = load i32, ptr %2, align 4
	%47 = icmp sgt i32 %46, 100
	%48 = icmp ne i1 %47, 0
	br i1 %48, label %label_44, label %label_45

label_44:
	%49 = load i32, ptr %22, align 4
	%50 = add nsw i32 %49, 9
	store i32 %50, ptr %22, align 4
	br label %label_43

label_45:
	br label %label_43

label_43:
	br label %label_38

label_38:
	%56 = load i32, ptr %2, align 4
	switch i32 %56, label %label_55 [
		 i32 0, label %label_52
		 i32 1, label %label_53
		 i32 2, label %label_53
		 i32 3, label %label_54
	]

label_52:
	call void @side_effect()
	br label %label_51

label_53:
	call void @side_effect()
	br label %label_51

label_54:
	%57 = alloca i32, align 4
	%58 = load i32, ptr %2, align 4
	%59 = mul nsw i32 %58, 2
	store i32 %59, ptr %57, align 4
	br label %label_51

label_55:
	call void @side_effect()
	br label %label_51

label_51:
	%64 = load i32, ptr %2, align 4
	%65 = srem i32 %64, 3
	switch i32 %65, label %label_60 [
		 i32 0, label %label_61
		 i32 1, label %label_62
		 i32 2, label %label_63
	]

label_61:
	call void @side_effect()
	br label %label_62

label_62:
	call void @side_effect()
	br label %label_63

label_63:
	call void @side_effect()
	br label %label_60

label_60:
	%66 = alloca i8, align 1
	%67 = load i32, ptr %2, align 4
	%68 = trunc i32 %67 to i8
	store i8 %68, ptr %66, align 1
	%72 = load i8, ptr %66, align 1
	switch i8 %72, label %label_71 [
		 i8 97, label %label_70
		 i8 101, label %label_70
		 i8 105, label %label_70
		 i8 111, label %label_70
		 i8 117, label %label_70
	]

label_70:
	call void @side_effect()
	br label %label_69

label_71:
	br label %label_69

label_69:
	br label %label_3

label_3:
	ret void
}

define dso_local void @test_iteration_statements() #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 0, ptr %2, align 4
	br label %label_4

label_4:
	%7 = load i32, ptr %2, align 4
	%8 = icmp slt i32 %7, 10
	%9 = icmp ne i1 %8, 0
	br i1 %9, label %label_5, label %label_6

label_5:
	%10 = load i32, ptr %2, align 4
	%11 = add nsw i32 %10, 2
	store i32 %11, ptr %2, align 4
	br label %label_4

label_6:
	store i32 0, ptr %2, align 4
	br label %label_12

label_12:
	%15 = load i32, ptr %2, align 4
	%16 = icmp slt i32 %15, 5
	%17 = icmp ne i1 %16, 0
	br i1 %17, label %label_13, label %label_14

label_13:
	%18 = load i32, ptr %3, align 4
	%19 = load i32, ptr %2, align 4
	%20 = add nsw i32 %18, %19
	store i32 %20, ptr %3, align 4
	br label %label_12

label_14:
	store i32 0, ptr %2, align 4
	br label %label_22

label_22:
	%23 = load i32, ptr %2, align 4
	%24 = add nsw i32 %23, 1
	store i32 %24, ptr %2, align 4
	%25 = load i32, ptr %2, align 4
	%26 = icmp slt i32 %25, 10
	%27 = icmp ne i1 %26, 0
	br i1 %27, label %label_22, label %label_21

label_21:
	store i32 0, ptr %2, align 4
	br label %label_29

label_29:
	%30 = load i32, ptr %2, align 4
	%31 = add nsw i32 %30, 1
	store i32 %31, ptr %2, align 4
	br label %label_28

label_28:
	store i32 0, ptr %3, align 4
	store i32 0, ptr %2, align 4
	br label %label_34

label_34:
	%35 = load i32, ptr %2, align 4
	%36 = icmp slt i32 %35, 10
	%37 = icmp ne i1 %36, 0
	br i1 %37, label %label_32, label %label_33

label_32:
	%38 = load i32, ptr %3, align 4
	%39 = load i32, ptr %2, align 4
	%40 = add nsw i32 %38, %39
	store i32 %40, ptr %3, align 4
	br label %label_34

label_33:
	%43 = alloca i32, align 4
	store i32 0, ptr %43, align 4
	br label %label_44

label_44:
	%45 = load i32, ptr %43, align 4
	%46 = icmp slt i32 %45, 5
	%47 = icmp ne i1 %46, 0
	br i1 %47, label %label_41, label %label_42

label_41:
	%48 = load i32, ptr %3, align 4
	%49 = load i32, ptr %43, align 4
	%50 = add nsw i32 %48, %49
	store i32 %50, ptr %3, align 4
	br label %label_44

label_42:
	br label %label_53

label_53:
	br label %label_51

label_51:
	%54 = load i32, ptr %3, align 4
	%55 = add nsw i32 %54, 1
	store i32 %55, ptr %3, align 4
	br label %label_52
	br label %label_53

label_52:
	store i32 0, ptr %2, align 4
	br label %label_58

label_58:
	%59 = load i32, ptr %2, align 4
	%60 = icmp slt i32 %59, 3
	%61 = icmp ne i1 %60, 0
	br i1 %61, label %label_56, label %label_57

label_56:
	%62 = load i32, ptr %3, align 4
	%63 = load i32, ptr %2, align 4
	%64 = add nsw i32 %62, %63
	store i32 %64, ptr %3, align 4
	br label %label_58

label_57:
	store i32 0, ptr %2, align 4
	br label %label_67

label_67:
	br label %label_65

label_65:
	%68 = load i32, ptr %3, align 4
	%69 = load i32, ptr %2, align 4
	%70 = add nsw i32 %68, %69
	store i32 %70, ptr %3, align 4
	%73 = load i32, ptr %2, align 4
	%74 = icmp sge i32 %73, 3
	%75 = icmp ne i1 %74, 0
	br i1 %75, label %label_72, label %label_71

label_72:
	br label %label_66

label_71:
	br label %label_67

label_66:
	store i32 0, ptr %3, align 4
	%78 = alloca i32, align 4
	store i32 0, ptr %78, align 4
	br label %label_79

label_79:
	%80 = load i32, ptr %78, align 4
	%81 = icmp slt i32 %80, 4
	%82 = icmp ne i1 %81, 0
	br i1 %82, label %label_76, label %label_77

label_76:
	%85 = alloca i32, align 4
	store i32 0, ptr %85, align 4
	br label %label_86

label_86:
	%87 = load i32, ptr %85, align 4
	%88 = icmp slt i32 %87, 4
	%89 = icmp ne i1 %88, 0
	br i1 %89, label %label_83, label %label_84

label_83:
	%92 = load i32, ptr %85, align 4
	%93 = icmp eq i32 %92, 2
	%94 = icmp ne i1 %93, 0
	br i1 %94, label %label_91, label %label_90

label_91:
	br label %label_90

label_90:
	%97 = load i32, ptr %78, align 4
	%98 = icmp eq i32 %97, 3
	%99 = icmp ne i1 %98, 0
	br i1 %99, label %label_96, label %label_95

label_96:
	br label %label_84

label_95:
	%100 = load i32, ptr %3, align 4
	%101 = load i32, ptr %78, align 4
	%102 = load i32, ptr %85, align 4
	%103 = mul nsw i32 %101, %102
	%104 = add nsw i32 %100, %103
	store i32 %104, ptr %3, align 4
	br label %label_86

label_84:
	br label %label_79

label_77:
	store i32 0, ptr %2, align 4
	br label %label_105

label_105:
	br label %label_106

label_106:
	%108 = load i32, ptr %3, align 4
	%109 = load i32, ptr %2, align 4
	%110 = add nsw i32 %108, %109
	store i32 %110, ptr %3, align 4
	%111 = load i32, ptr %2, align 4
	%112 = add nsw i32 %111, 1
	store i32 %112, ptr %2, align 4
	%115 = load i32, ptr %2, align 4
	%116 = icmp sgt i32 %115, 5
	%117 = icmp ne i1 %116, 0
	br i1 %117, label %label_114, label %label_113

label_114:
	br label %label_107

label_113:
	br label %label_105

label_107:
	store i32 0, ptr %2, align 4
	store i32 0, ptr %3, align 4
	br label %label_118

label_118:
	%121 = load i32, ptr %2, align 4
	%122 = icmp slt i32 %121, 10
	%123 = icmp ne i1 %122, 0
	br i1 %123, label %label_119, label %label_120

label_119:
	%126 = load i32, ptr %2, align 4
	%127 = srem i32 %126, 2
	%128 = icmp eq i32 %127, 0
	%129 = icmp ne i1 %128, 0
	br i1 %129, label %label_125, label %label_124

label_125:
	br label %label_124

label_124:
	%130 = load i32, ptr %3, align 4
	%131 = load i32, ptr %2, align 4
	%132 = add nsw i32 %130, %131
	store i32 %132, ptr %3, align 4
	br label %label_118

label_120:
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_jump_statements(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = alloca i32, align 4
	store i32 0, ptr %4, align 4
	%7 = load i32, ptr %2, align 4
	%8 = icmp slt i32 %7, 0
	%9 = icmp ne i1 %8, 0
	br i1 %9, label %label_6, label %label_5

label_6:
	br label %label_10

label_5:
	store i32 1, ptr %4, align 4
	br label %label_11
	br label %label_10

label_10:
	store i32 -1, ptr %4, align 4
	br label %label_11

label_11:
	%12 = alloca i32, align 4
	store i32 0, ptr %12, align 4
	br label %label_13

label_13:
	%16 = load i32, ptr %12, align 4
	%17 = icmp slt i32 %16, 5
	%18 = icmp ne i1 %17, 0
	br i1 %18, label %label_15, label %label_14

label_15:
	br label %label_13

label_14:
	%21 = load i32, ptr %2, align 4
	%22 = icmp eq i32 %21, 999
	%23 = icmp ne i1 %22, 0
	br i1 %23, label %label_20, label %label_19

label_20:
	br label %label_3

label_19:
	br label %label_3

label_3:
	ret void
}

define dso_local i32 @test_return_value(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = alloca i32, align 4
	%7 = load i32, ptr %2, align 4
	%8 = icmp sgt i32 %7, 0
	%9 = icmp ne i1 %8, 0
	br i1 %9, label %label_6, label %label_5

label_6:
	store i32 %-20000, ptr %4, align 4
	br label %label_3

label_5:
	%12 = load i32, ptr %2, align 4
	%13 = icmp slt i32 %12, 0
	%14 = icmp ne i1 %13, 0
	br i1 %14, label %label_11, label %label_10

label_11:
	store i32 %-10000, ptr %4, align 4
	br label %label_3

label_10:
	store i32 0, ptr %4, align 4
	br label %label_3

label_3:
	%15 = load i32, ptr %4, align 4
	ret i32 %15
}

define dso_local void @test_labeled_statements(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	br label %label_4

label_4:
	call void @side_effect()
	br label %label_5

label_5:
	br label %label_6

label_6:
	%7 = alloca i32, align 4
	%8 = load i32, ptr %2, align 4
	store i32 %8, ptr %7, align 4
	%11 = load i32, ptr %2, align 4
	%12 = icmp slt i32 %11, 0
	%13 = icmp ne i1 %12, 0
	br i1 %13, label %label_10, label %label_9

label_10:
	br label %label_4

label_9:
	%16 = load i32, ptr %2, align 4
	%17 = icmp sgt i32 %16, 0
	%18 = icmp ne i1 %17, 0
	br i1 %18, label %label_15, label %label_14

label_15:
	br label %label_5

label_14:
	br label %label_6
	br label %label_3

label_3:
	ret void
}

define dso_local void @test_declaration_statements() #0 {
	%2 = alloca i32, align 4
	store i32 5, ptr %2, align 4
	%3 = alloca i32, align 4
	store i32 42, ptr %3, align 4
	%4 = alloca [8 x i32], align 4
	%5 = alloca ptr, align 8
	%6 = load i32, ptr %4, align 4
	store ptr %6, ptr %5, align 8
	%9 = alloca i32, align 4
	store i32 0, ptr %9, align 4
	%10 = alloca i32, align 4
	store i32 10, ptr %10, align 4
	br label %label_11

label_11:
	%12 = load i32, ptr %9, align 4
	%13 = load i32, ptr %10, align 4
	%14 = icmp slt i32 %12, %13
	%15 = icmp ne i1 %14, 0
	br i1 %15, label %label_7, label %label_8

label_7:
	call void @side_effect()
	br label %label_11

label_8:
	%16 = alloca i8, align 1
	store i8 1, ptr %16, align 1
	%19 = icmp ne i8 %-20000, 0
	br i1 %19, label %label_18, label %label_17

label_18:
	call void @side_effect()
	br label %label_17

label_17:
	%20 = alloca ptr, align 8
	%21 = alloca [3 x i32], align 4
	%22 = getelementptr inbounds [3 x i32], ptr %21, i64 0, i64 0
	store i32 1, ptr %22, align 4
	%23 = getelementptr inbounds [3 x i32], ptr %21, i64 0, i64 1
	store i32 1, ptr %23, align 4
	%24 = getelementptr inbounds [3 x i32], ptr %21, i64 0, i64 2
	store i32 1, ptr %24, align 4
	store ptr %21, ptr %20, align 8
	%25 = alloca [5 x i32], align 4
	%26 = getelementptr inbounds [5 x i32], ptr %25, i64 0, i64 0
	store i32 1, ptr %26, align 4
	%27 = getelementptr inbounds [5 x i32], ptr %25, i64 0, i64 4
	store i32 1, ptr %27, align 4
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_edge_cases() #0 {
	%2 = alloca i32, align 4
	%3 = load i32, ptr %-100, align 4
	store i32 %3, ptr %2, align 4
	%6 = load i32, ptr %2, align 4
	%7 = icmp eq i32 %6, 0
	%8 = icmp ne i1 %7, 0
	br i1 %8, label %label_5, label %label_4

label_5:
	%11 = load i32, ptr %2, align 4
	%12 = icmp eq i32 %11, 0
	%13 = icmp ne i1 %12, 0
	br i1 %13, label %label_10, label %label_9

label_10:
	%16 = load i32, ptr %2, align 4
	%17 = icmp eq i32 %16, 0
	%18 = icmp ne i1 %17, 0
	br i1 %18, label %label_15, label %label_14

label_15:
	%21 = load i32, ptr %2, align 4
	%22 = icmp eq i32 %21, 0
	%23 = icmp ne i1 %22, 0
	br i1 %23, label %label_20, label %label_19

label_20:
	call void @side_effect()
	br label %label_19

label_19:
	br label %label_14

label_14:
	br label %label_9

label_9:
	br label %label_4

label_4:
	%26 = load i32, ptr %2, align 4
	%27 = icmp sge i32 %26, 0
	%28 = icmp ne i1 %27, 0
	br i1 %28, label %label_25, label %label_24

label_25:
	%29 = alloca i32, align 4
	store i32 0, ptr %29, align 4
	br label %label_30

label_30:
	%33 = load i32, ptr %29, align 4
	%34 = icmp slt i32 %33, 3
	%35 = icmp ne i1 %34, 0
	br i1 %35, label %label_31, label %label_32

label_31:
	%40 = load i32, ptr %29, align 4
	switch i32 %40, label %label_39 [
		 i32 0, label %label_37
		 i32 1, label %label_38
	]

label_37:
	br label %label_36

label_38:
	br label %label_39

label_39:
	br label %label_36

label_36:
	br label %label_30

label_32:
	br label %label_24

label_24:
	%43 = alloca i32, align 4
	store i32 0, ptr %43, align 4
	%44 = alloca i32, align 4
	store i32 10, ptr %44, align 4
	br label %label_45

label_45:
	%46 = load i32, ptr %43, align 4
	%47 = load i32, ptr %44, align 4
	%48 = icmp slt i32 %46, %47
	%49 = icmp ne i1 %48, 0
	br i1 %49, label %label_41, label %label_42

label_41:
	br label %label_45

label_42:
	%50 = alloca i32, align 4
	%52 = load i32, ptr %2, align 4
	switch i32 %52, label %label_51 [
	]

label_51:
	%55 = load i32, ptr %2, align 4
	switch i32 %55, label %label_54 [
	]

label_54:
	br label %label_53

label_53:
	br label %label_56
	br label %label_56

label_56:
	br label %label_1

label_1:
	ret void
}

define dso_local i32 @main() #0 {
	%2 = alloca i32, align 4
	call void @test_expression_statements()
	call void @test_compound_statements()
	call void @test_selection_statements(i32 noundef 0)
	call void @test_selection_statements(i32 noundef 1)
	call void @test_selection_statements(i32 noundef -1)
	call void @test_selection_statements(i32 noundef 42)
	call void @test_iteration_statements()
	call void @test_jump_statements(i32 noundef 1)
	call void @test_jump_statements(i32 noundef -1)
	call void @test_jump_statements(i32 noundef 0)
	call void @test_labeled_statements(i32 noundef 0)
	call void @test_declaration_statements()
	call void @test_edge_cases()
	%3 = alloca i32, align 4
	%4 = call i32 @test_return_value(i32 noundef 5)
	store i32 %4, ptr %3, align 4
	%5 = call i32 @test_return_value(i32 noundef -3)
	store i32 %5, ptr %3, align 4
	%6 = call i32 @test_return_value(i32 noundef 0)
	store i32 %6, ptr %3, align 4
	store i32 0, ptr %2, align 4
	br label %label_1

label_1:
	%7 = load i32, ptr %2, align 4
	ret i32 %7
}


attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}