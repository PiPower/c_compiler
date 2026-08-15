%struct._IO_FILE = type { i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i32, i64, i16, i8, [1 x i8], ptr, i64, ptr, ptr, ptr, ptr, i64, i32, [20 x i8] }
%struct.anon.2 = type { i32, i32 }
%struct.anon.3 = type { i64, i64 }
%struct.anon.4 = type { i64, i64 }

@g = internal global i32 0, align 4
@stderr = external global ptr
@stdout = external global ptr
@stdin = external global ptr

define internal void @side_effect() #0 {
	%1 = load i32, ptr @g, align 4
	%2 = add nsw i32 %1, 1
	store i32 %2, ptr @g, align 4
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_expression_statements() #0 {
	%1 = alloca i32, align 4
	store i32 42, ptr %1, align 4
	%2 = load i32, ptr %1, align 4
	%3 = add nsw i32 %2, 1
	store i32 %3, ptr %1, align 4
	%4 = load i32, ptr %1, align 4
	%5 = sub nsw i32 %4, 1
	store i32 %5, ptr %1, align 4
	%6 = load i32, ptr %1, align 4
	%7 = mul nsw i32 %6, 2
	store i32 %7, ptr %1, align 4
	%8 = load i32, ptr %1, align 4
	%9 = sdiv i32 %8, 2
	store i32 %9, ptr %1, align 4
	%10 = load i32, ptr %1, align 4
	%11 = srem i32 %10, 7
	store i32 %11, ptr %1, align 4
	%12 = load i32, ptr %1, align 4
	%13 = and i32 %12, 255
	store i32 %13, ptr %1, align 4
	%14 = load i32, ptr %1, align 4
	%15 = or i32 %14, 1
	store i32 %15, ptr %1, align 4
	%16 = load i32, ptr %1, align 4
	%17 = xor i32 %16, 15
	store i32 %17, ptr %1, align 4
	%18 = load i32, ptr %1, align 4
	%19 = shl i32 %18, 2
	store i32 %19, ptr %1, align 4
	%20 = load i32, ptr %1, align 4
	%21 = ashr i32 %20, 1
	store i32 %21, ptr %1, align 4
	%22 = load i32, ptr %1, align 4
	%23 = add nsw i32 %22, 1
	store i32 %23, ptr %1, align 4
	%24 = load i32, ptr %1, align 4
	%25 = add nsw i32 %24, 1
	store i32 %25, ptr %1, align 4
	%26 = load i32, ptr %1, align 4
	%27 = sub nsw i32 %26, 1
	store i32 %27, ptr %1, align 4
	%28 = load i32, ptr %1, align 4
	%29 = sub nsw i32 %28, 1
	store i32 %29, ptr %1, align 4
	call void @side_effect()
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_compound_statements() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	store i32 1, ptr %1, align 4
	store i32 2, ptr %2, align 4
	store i32 3, ptr %3, align 4
	store i32 0, ptr %4, align 4
	%6 = load i32, ptr %4, align 4
	%7 = add nsw i32 %6, 1
	store i32 %7, ptr %4, align 4
	%8 = load i32, ptr %4, align 4
	%9 = mul nsw i32 %8, 2
	store i32 %9, ptr %5, align 4
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_selection_statements(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca double, align 8
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	%6 = alloca i8, align 1
	store i32 %0, ptr %2, align 4
	store double 0x0000000000000000, ptr %3, align 8
	%7 = load i32, ptr %2, align 4
	%8 = icmp sgt i32 %7, 0
	%9 = icmp ne i1 %8, 0
	br i1 %9, label %label_6, label %label_5

label_6:
	%10 = load double, ptr %3, align 8
	%11 = fadd double %10, 1.000000
	store double %11, ptr %3, align 8
	br label %label_5

label_5:
	%12 = load i32, ptr %2, align 4
	%13 = icmp sge i32 %12, 0
	%14 = icmp ne i1 %13, 0
	br i1 %14, label %label_13, label %label_14

label_13:
	%15 = load double, ptr %3, align 8
	%16 = fadd double %15, 2.000000
	store double %16, ptr %3, align 8
	br label %label_12

label_14:
	%17 = load double, ptr %3, align 8
	%18 = fadd double %17, 3.000000
	store double %18, ptr %3, align 8
	br label %label_12

label_12:
	store i32 11, ptr %4, align 4
	%19 = load i32, ptr %2, align 4
	%20 = icmp slt i32 %19, 0
	%21 = icmp ne i1 %20, 0
	br i1 %21, label %label_24, label %label_25

label_24:
	%22 = load i32, ptr %4, align 4
	%23 = sub nsw i32 %22, 1
	store i32 %23, ptr %4, align 4
	br label %label_23

label_25:
	%24 = load i32, ptr %2, align 4
	%25 = icmp eq i32 %24, 0
	%26 = icmp ne i1 %25, 0
	br i1 %26, label %label_31, label %label_32

label_31:
	%27 = load i32, ptr %4, align 4
	%28 = mul nsw i32 %27, 32
	store i32 %28, ptr %4, align 4
	br label %label_23

label_32:
	%29 = load i32, ptr %4, align 4
	%30 = sdiv i32 %29, 2
	store i32 %30, ptr %4, align 4
	br label %label_23

label_23:
	%31 = load i32, ptr %2, align 4
	%32 = icmp sle i32 %31, 0
	%33 = icmp ne i1 %32, 0
	br i1 %33, label %label_41, label %label_40

label_41:
	%34 = load i32, ptr %2, align 4
	%35 = icmp sgt i32 %34, 100
	%36 = icmp ne i1 %35, 0
	br i1 %36, label %label_46, label %label_47

label_46:
	%37 = load i32, ptr %4, align 4
	%38 = add nsw i32 %37, 9
	store i32 %38, ptr %4, align 4
	br label %label_45

label_47:
	%39 = load i32, ptr %4, align 4
	%40 = sub nsw i32 %39, 21
	store i32 %40, ptr %4, align 4
	br label %label_45

label_45:
	br label %label_40

label_40:
	%41 = load i32, ptr %2, align 4
	switch i32 %41, label %label_59 [
		 i32 0, label %label_56
		 i32 1, label %label_57
		 i32 2, label %label_57
		 i32 3, label %label_58
	]

label_56:
	call void @side_effect()
	br label %label_55

label_57:
	call void @side_effect()
	br label %label_55

label_58:
	%42 = load i32, ptr %2, align 4
	%43 = mul nsw i32 %42, 2
	store i32 %43, ptr %5, align 4
	br label %label_55

label_59:
	call void @side_effect()
	br label %label_55

label_55:
	%44 = load i32, ptr %2, align 4
	%45 = srem i32 %44, 3
	switch i32 %45, label %label_64 [
		 i32 0, label %label_65
		 i32 1, label %label_66
		 i32 2, label %label_67
	]

label_65:
	call void @side_effect()
	br label %label_66

label_66:
	call void @side_effect()
	br label %label_67

label_67:
	call void @side_effect()
	br label %label_64

label_64:
	%46 = load i32, ptr %2, align 4
	%47 = trunc i32 %46 to i8
	store i8 %47, ptr %6, align 1
	%48 = load i8, ptr %6, align 1
	switch i8 %48, label %label_75 [
		 i8 97, label %label_74
		 i8 101, label %label_74
		 i8 105, label %label_74
		 i8 111, label %label_74
		 i8 117, label %label_74
	]

label_74:
	call void @side_effect()
	br label %label_73

label_75:
	br label %label_73

label_73:
	br label %label_3

label_3:
	ret void
}

define dso_local void @test_iteration_statements() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	store i32 0, ptr %2, align 4
	store i32 0, ptr %1, align 4
	br label %label_4

label_4:
	%6 = load i32, ptr %1, align 4
	%7 = icmp slt i32 %6, 10
	%8 = icmp ne i1 %7, 0
	br i1 %8, label %label_5, label %label_6

label_5:
	%9 = load i32, ptr %1, align 4
	%10 = add nsw i32 %9, 2
	store i32 %10, ptr %1, align 4
	br label %label_4

label_6:
	store i32 0, ptr %1, align 4
	br label %label_12

label_12:
	%11 = load i32, ptr %1, align 4
	%12 = icmp slt i32 %11, 5
	%13 = icmp ne i1 %12, 0
	br i1 %13, label %label_13, label %label_14

label_13:
	%14 = load i32, ptr %2, align 4
	%15 = load i32, ptr %1, align 4
	%16 = add nsw i32 %14, %15
	store i32 %16, ptr %2, align 4
	%17 = load i32, ptr %1, align 4
	%18 = add nsw i32 %17, 1
	store i32 %18, ptr %1, align 4
	br label %label_12

label_14:
	store i32 0, ptr %1, align 4
	br label %label_24

label_24:
	%19 = load i32, ptr %1, align 4
	%20 = add nsw i32 %19, 1
	store i32 %20, ptr %1, align 4
	%21 = load i32, ptr %1, align 4
	%22 = icmp slt i32 %21, 10
	%23 = icmp ne i1 %22, 0
	br i1 %23, label %label_24, label %label_23

label_23:
	store i32 0, ptr %1, align 4
	br label %label_31

label_31:
	%24 = load i32, ptr %1, align 4
	%25 = add nsw i32 %24, 1
	store i32 %25, ptr %1, align 4
	br label %label_30

label_30:
	store i32 0, ptr %2, align 4
	store i32 0, ptr %1, align 4
	br label %label_37

label_37:
	%26 = load i32, ptr %1, align 4
	%27 = icmp slt i32 %26, 10
	%28 = icmp ne i1 %27, 0
	br i1 %28, label %label_34, label %label_35

label_34:
	%29 = load i32, ptr %2, align 4
	%30 = load i32, ptr %1, align 4
	%31 = add nsw i32 %29, %30
	store i32 %31, ptr %2, align 4
	br label %label_36

label_36:
	%32 = load i32, ptr %1, align 4
	%33 = add nsw i32 %32, 1
	store i32 %33, ptr %1, align 4
	br label %label_37

label_35:
	store i32 0, ptr %3, align 4
	br label %label_50

label_50:
	%34 = load i32, ptr %3, align 4
	%35 = icmp slt i32 %34, 5
	%36 = icmp ne i1 %35, 0
	br i1 %36, label %label_46, label %label_47

label_46:
	%37 = load i32, ptr %2, align 4
	%38 = load i32, ptr %3, align 4
	%39 = add nsw i32 %37, %38
	store i32 %39, ptr %2, align 4
	br label %label_48

label_48:
	%40 = load i32, ptr %3, align 4
	%41 = add nsw i32 %40, 1
	store i32 %41, ptr %3, align 4
	br label %label_50

label_47:
	br label %label_62

label_62:
	br label %label_59

label_59:
	%42 = load i32, ptr %2, align 4
	%43 = add nsw i32 %42, 1
	store i32 %43, ptr %2, align 4
	br label %label_60

label_61:
	br label %label_62

label_60:
	store i32 0, ptr %1, align 4
	br label %label_68

label_68:
	%44 = load i32, ptr %1, align 4
	%45 = icmp slt i32 %44, 3
	%46 = icmp ne i1 %45, 0
	br i1 %46, label %label_65, label %label_66

label_65:
	%47 = load i32, ptr %1, align 4
	%48 = add nsw i32 %47, 1
	store i32 %48, ptr %1, align 4
	%49 = load i32, ptr %2, align 4
	%50 = load i32, ptr %1, align 4
	%51 = add nsw i32 %49, %50
	store i32 %51, ptr %2, align 4
	br label %label_67

label_67:
	br label %label_68

label_66:
	store i32 0, ptr %1, align 4
	br label %label_80

label_80:
	br label %label_77

label_77:
	%52 = load i32, ptr %2, align 4
	%53 = load i32, ptr %1, align 4
	%54 = add nsw i32 %52, %53
	store i32 %54, ptr %2, align 4
	%55 = load i32, ptr %1, align 4
	%56 = icmp sge i32 %55, 3
	%57 = icmp ne i1 %56, 0
	br i1 %57, label %label_85, label %label_84

label_85:
	br label %label_78

label_84:
	br label %label_79

label_79:
	%58 = load i32, ptr %1, align 4
	%59 = add nsw i32 %58, 1
	store i32 %59, ptr %1, align 4
	br label %label_80

label_78:
	store i32 0, ptr %2, align 4
	store i32 0, ptr %4, align 4
	br label %label_95

label_95:
	%60 = load i32, ptr %4, align 4
	%61 = icmp slt i32 %60, 4
	%62 = icmp ne i1 %61, 0
	br i1 %62, label %label_91, label %label_92

label_91:
	store i32 0, ptr %5, align 4
	br label %label_103

label_103:
	%63 = load i32, ptr %5, align 4
	%64 = icmp slt i32 %63, 4
	%65 = icmp ne i1 %64, 0
	br i1 %65, label %label_99, label %label_100

label_99:
	%66 = load i32, ptr %5, align 4
	%67 = icmp eq i32 %66, 2
	%68 = icmp ne i1 %67, 0
	br i1 %68, label %label_108, label %label_107

label_108:
	br label %label_101

label_107:
	%69 = load i32, ptr %4, align 4
	%70 = icmp eq i32 %69, 3
	%71 = icmp ne i1 %70, 0
	br i1 %71, label %label_113, label %label_112

label_113:
	br label %label_100

label_112:
	%72 = load i32, ptr %2, align 4
	%73 = load i32, ptr %4, align 4
	%74 = load i32, ptr %5, align 4
	%75 = mul nsw i32 %73, %74
	%76 = add nsw i32 %72, %75
	store i32 %76, ptr %2, align 4
	br label %label_101

label_101:
	%77 = load i32, ptr %5, align 4
	%78 = add nsw i32 %77, 1
	store i32 %78, ptr %5, align 4
	br label %label_103

label_100:
	br label %label_93

label_93:
	%79 = load i32, ptr %4, align 4
	%80 = add nsw i32 %79, 1
	store i32 %80, ptr %4, align 4
	br label %label_95

label_92:
	store i32 0, ptr %1, align 4
	br label %label_126

label_126:
	br label %label_127

label_127:
	%81 = load i32, ptr %2, align 4
	%82 = load i32, ptr %1, align 4
	%83 = add nsw i32 %81, %82
	store i32 %83, ptr %2, align 4
	%84 = load i32, ptr %1, align 4
	%85 = add nsw i32 %84, 1
	store i32 %85, ptr %1, align 4
	%86 = load i32, ptr %1, align 4
	%87 = icmp sgt i32 %86, 5
	%88 = icmp ne i1 %87, 0
	br i1 %88, label %label_135, label %label_134

label_135:
	br label %label_128

label_134:
	br label %label_126

label_128:
	store i32 0, ptr %1, align 4
	store i32 0, ptr %2, align 4
	br label %label_139

label_139:
	%89 = load i32, ptr %1, align 4
	%90 = icmp slt i32 %89, 10
	%91 = icmp ne i1 %90, 0
	br i1 %91, label %label_140, label %label_141

label_140:
	%92 = load i32, ptr %1, align 4
	%93 = add nsw i32 %92, 1
	store i32 %93, ptr %1, align 4
	%94 = load i32, ptr %1, align 4
	%95 = srem i32 %94, 2
	%96 = icmp eq i32 %95, 0
	%97 = icmp ne i1 %96, 0
	br i1 %97, label %label_148, label %label_147

label_148:
	br label %label_139

label_147:
	%98 = load i32, ptr %2, align 4
	%99 = load i32, ptr %1, align 4
	%100 = add nsw i32 %98, %99
	store i32 %100, ptr %2, align 4
	br label %label_139

label_141:
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_jump_statements(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	store i32 0, ptr %3, align 4
	%5 = load i32, ptr %2, align 4
	%6 = icmp slt i32 %5, 0
	%7 = icmp ne i1 %6, 0
	br i1 %7, label %label_6, label %label_5

label_6:
	br label %label_10

label_5:
	store i32 1, ptr %3, align 4
	br label %label_11

label_10:
	store i32 -1, ptr %3, align 4
	br label %label_11

label_11:
	store i32 0, ptr %4, align 4
	br label %label_13

label_13:
	%8 = load i32, ptr %4, align 4
	%9 = icmp slt i32 %8, 5
	%10 = icmp ne i1 %9, 0
	br i1 %10, label %label_15, label %label_14

label_15:
	%11 = load i32, ptr %4, align 4
	%12 = add nsw i32 %11, 1
	store i32 %12, ptr %4, align 4
	br label %label_13

label_14:
	%13 = load i32, ptr %2, align 4
	%14 = icmp eq i32 %13, 999
	%15 = icmp ne i1 %14, 0
	br i1 %15, label %label_22, label %label_21

label_22:
	br label %label_3

label_21:
	br label %label_3

label_3:
	ret void
}

define dso_local i32 @test_return_value(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = load i32, ptr %2, align 4
	%5 = icmp sgt i32 %4, 0
	%6 = icmp ne i1 %5, 0
	br i1 %6, label %label_6, label %label_5

label_6:
	%7 = load i32, ptr %2, align 4
	store i32 %7, ptr %3, align 4
	br label %label_3

label_5:
	%8 = load i32, ptr %2, align 4
	%9 = icmp slt i32 %8, 0
	%10 = icmp ne i1 %9, 0
	br i1 %10, label %label_12, label %label_11

label_12:
	%11 = load i32, ptr %2, align 4
	%12 = mul nsw i32 -1, %11
	store i32 %12, ptr %3, align 4
	br label %label_3

label_11:
	store i32 0, ptr %3, align 4
	br label %label_3

label_3:
	%13 = load i32, ptr %3, align 4
	ret i32 %13
}

define dso_local void @test_labeled_statements(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	br label %label_4

label_4:
	call void @side_effect()
	br label %label_5

label_5:
	br label %label_6

label_6:
	%4 = load i32, ptr %2, align 4
	store i32 %4, ptr %3, align 4
	%5 = load i32, ptr %2, align 4
	%6 = icmp slt i32 %5, 0
	%7 = icmp ne i1 %6, 0
	br i1 %7, label %label_10, label %label_9

label_10:
	br label %label_4

label_9:
	%8 = load i32, ptr %2, align 4
	%9 = icmp sgt i32 %8, 0
	%10 = icmp ne i1 %9, 0
	br i1 %10, label %label_15, label %label_14

label_15:
	br label %label_5

label_14:
	br label %label_6

label_3:
	ret void
}

define dso_local void @test_declaration_statements() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca [8 x i32], align 4
	%4 = alloca ptr, align 8
	%5 = alloca i32, align 4
	%6 = alloca i32, align 4
	%7 = alloca i8, align 1
	%8 = alloca ptr, align 8
	%9 = alloca [3 x i32], align 4
	%10 = alloca [5 x i32], align 4
	store i32 5, ptr %1, align 4
	store i32 42, ptr %2, align 4
	%11 = getelementptr inbounds [8 x i32], ptr %3, i64 0, i64 0
	store ptr %11, ptr %4, align 8
	%12 = load ptr, ptr %4, align 8
	store i32 1, ptr %12, align 4
	store i32 0, ptr %5, align 4
	store i32 10, ptr %6, align 4
	br label %label_13

label_13:
	%13 = load i32, ptr %5, align 4
	%14 = load i32, ptr %6, align 4
	%15 = icmp slt i32 %13, %14
	%16 = icmp ne i1 %15, 0
	br i1 %16, label %label_8, label %label_9

label_8:
	call void @side_effect()
	br label %label_10

label_10:
	%17 = load i32, ptr %5, align 4
	%18 = add nsw i32 %17, 1
	store i32 %18, ptr %5, align 4
	%19 = load i32, ptr %6, align 4
	%20 = sub nsw i32 %19, 1
	store i32 %20, ptr %6, align 4
	br label %label_13

label_9:
	store i8 1, ptr %7, align 1
	%21 = load i8, ptr %7, align 1
	%22 = icmp ne i8 %21, 0
	br i1 %22, label %label_24, label %label_23

label_24:
	call void @side_effect()
	br label %label_23

label_23:
	call void @llvm.memset.p0.i64(ptr align 4 %9, i8 0, i64 12, i1 false)
	%23 = getelementptr inbounds [3 x i32], ptr %9, i64 0, i64 0
	store i32 1, ptr %23, align 4
	%24 = getelementptr inbounds [3 x i32], ptr %9, i64 0, i64 1
	store i32 2, ptr %24, align 4
	%25 = getelementptr inbounds [3 x i32], ptr %9, i64 0, i64 2
	store i32 3, ptr %25, align 4
	store ptr %9, ptr %8, align 8
	call void @llvm.memset.p0.i64(ptr align 4 %10, i8 0, i64 20, i1 false)
	%26 = getelementptr inbounds [5 x i32], ptr %10, i64 0, i64 0
	store i32 1, ptr %26, align 4
	%27 = getelementptr inbounds [5 x i32], ptr %10, i64 0, i64 4
	store i32 5, ptr %27, align 4
	%28 = getelementptr inbounds [5 x i32], ptr %10, i64 0, i64 0
	%29 = getelementptr inbounds i32, ptr %28, i64 4
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_edge_cases() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	%6 = load i32, ptr @g, align 4
	store i32 %6, ptr %1, align 4
	%7 = load i32, ptr %1, align 4
	%8 = icmp eq i32 %7, 0
	%9 = icmp ne i1 %8, 0
	br i1 %9, label %label_5, label %label_4

label_5:
	%10 = load i32, ptr %1, align 4
	%11 = icmp eq i32 %10, 0
	%12 = icmp ne i1 %11, 0
	br i1 %12, label %label_10, label %label_9

label_10:
	%13 = load i32, ptr %1, align 4
	%14 = icmp eq i32 %13, 0
	%15 = icmp ne i1 %14, 0
	br i1 %15, label %label_15, label %label_14

label_15:
	%16 = load i32, ptr %1, align 4
	%17 = icmp eq i32 %16, 0
	%18 = icmp ne i1 %17, 0
	br i1 %18, label %label_20, label %label_19

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
	%19 = load i32, ptr %1, align 4
	%20 = icmp sge i32 %19, 0
	%21 = icmp ne i1 %20, 0
	br i1 %21, label %label_25, label %label_24

label_25:
	store i32 0, ptr %2, align 4
	br label %label_30

label_30:
	%22 = load i32, ptr %2, align 4
	%23 = icmp slt i32 %22, 3
	%24 = icmp ne i1 %23, 0
	br i1 %24, label %label_31, label %label_32

label_31:
	%25 = load i32, ptr %2, align 4
	switch i32 %25, label %label_39 [
		 i32 0, label %label_37
		 i32 1, label %label_38
	]

label_37:
	br label %label_36

label_38:
	%26 = load i32, ptr %2, align 4
	%27 = add nsw i32 %26, 1
	store i32 %27, ptr %2, align 4
	br label %label_30

label_39:
	br label %label_36

label_36:
	%28 = load i32, ptr %2, align 4
	%29 = add nsw i32 %28, 1
	store i32 %29, ptr %2, align 4
	br label %label_30

label_32:
	br label %label_24

label_24:
	store i32 0, ptr %3, align 4
	store i32 10, ptr %4, align 4
	br label %label_50

label_50:
	%30 = load i32, ptr %3, align 4
	%31 = load i32, ptr %4, align 4
	%32 = icmp slt i32 %30, %31
	%33 = icmp ne i1 %32, 0
	br i1 %33, label %label_45, label %label_46

label_45:
	br label %label_47

label_47:
	%34 = load i32, ptr %3, align 4
	%35 = add nsw i32 %34, 1
	store i32 %35, ptr %3, align 4
	%36 = load i32, ptr %4, align 4
	%37 = sub nsw i32 %36, 1
	store i32 %37, ptr %4, align 4
	br label %label_50

label_46:
	%38 = load i32, ptr %1, align 4
	%39 = icmp sgt i32 %38, 0
	%40 = icmp ne i1 %39, 0
	br i1 %40, label %label_60, label %label_61

label_60:
	%41 = load i32, ptr %1, align 4
	%42 = icmp sgt i32 %41, 10
	%43 = icmp ne i1 %42, 0
	br i1 %43, label %label_66, label %label_67

label_66:
	br label %label_68

label_67:
	br label %label_68

label_68:
	%44 = phi i32 [  2, %label_66 ], [  1, %label_67 ]
	br label %label_62

label_61:
	br label %label_62

label_62:
	%45 = phi i32 [ %44, %label_68 ], [  0, %label_61 ]
	store i32 %45, ptr %5, align 4
	%46 = load i32, ptr %1, align 4
	switch i32 %46, label %label_74 [
	]

label_74:
	%47 = load i32, ptr %1, align 4
	switch i32 %47, label %label_77 [
	]

label_77:
	br label %label_76

label_76:
	br label %label_79

label_79:
	br label %label_1

label_1:
	ret void
}

define dso_local i32 @main() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	store i32 0, ptr %1, align 4
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
	%3 = call i32 @test_return_value(i32 noundef 5)
	store i32 %3, ptr %2, align 4
	%4 = call i32 @test_return_value(i32 noundef -3)
	store i32 %4, ptr %2, align 4
	%5 = call i32 @test_return_value(i32 noundef 0)
	store i32 %5, ptr %2, align 4
	store i32 0, ptr %1, align 4
	br label %label_1

label_1:
	%6 = load i32, ptr %1, align 4
	ret i32 %6
}

declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }