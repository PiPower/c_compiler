%struct.XD = type { [4 x [2 x ptr]] }
%struct.Point = type { double, double }
%struct.Flags = type { i32, i32, i32 }
%struct.Line = type { %struct.Point, %struct.Point }
%union.Data = type { i32 }

@.str.1 = private unnamed_addr constant [3 x i8] c"xd\00", align 1
@.str.2 = private unnamed_addr constant [6 x i8] c"pt\09ro\00", align 1
@.str.3 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@.str.4 = private unnamed_addr constant [4 x i8] c"foo\00", align 1
@word = dso_local global [2 x ptr] [ptr@.str.1, ptr@.str.2], align 8
@test_array333 = dso_local global [3 x [4 x float]] [[4 x float] [float 60000.000000, float 3.000000, float 0.0e+00, float 0.0e+00], [4 x float] zeroinitializer, [4 x float] [float 4.000000, float 2.000000, float 0.0e+00, float 0.0e+00]], align 4
@test_array = dso_local global [8 x [3 x [2 x i32]]] [[3 x [2 x i32]] zeroinitializer, [3 x [2 x i32]] zeroinitializer, [3 x [2 x i32]] [[2 x i32] [i32 3, i32 4], [2 x i32] [i32 5, i32 0], [2 x i32] zeroinitializer], [3 x [2 x i32]] [[2 x i32] [i32 100, i32 1000], [2 x i32] [i32 100, i32 0], [2 x i32] zeroinitializer], [3 x [2 x i32]] zeroinitializer, [3 x [2 x i32]] [[2 x i32] [i32 1, i32 2], [2 x i32] [i32 3, i32 0], [2 x i32] zeroinitializer], [3 x [2 x i32]] zeroinitializer, [3 x [2 x i32]] [[2 x i32] [i32 1, i32 2], [2 x i32] [i32 3, i32 0], [2 x i32] zeroinitializer]], align 4
@test_array999 = dso_local global [11 x [7 x [3 x [2 x i64]]]] [[7 x [3 x [2 x i64]]] zeroinitializer, [7 x [3 x [2 x i64]]] zeroinitializer, [7 x [3 x [2 x i64]]] [[3 x [2 x i64]] [[2 x i64] [i64 3, i64 4], [2 x i64] [i64 5, i64 0], [2 x i64] zeroinitializer], [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer], [7 x [3 x [2 x i64]]] [[3 x [2 x i64]] [[2 x i64] [i64 100, i64 1000], [2 x i64] [i64 100, i64 0], [2 x i64] zeroinitializer], [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer], [7 x [3 x [2 x i64]]] zeroinitializer, [7 x [3 x [2 x i64]]] [[3 x [2 x i64]] [[2 x i64] [i64 1, i64 2], [2 x i64] [i64 3, i64 0], [2 x i64] zeroinitializer], [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer], [7 x [3 x [2 x i64]]] zeroinitializer, [7 x [3 x [2 x i64]]] [[3 x [2 x i64]] [[2 x i64] [i64 1, i64 2], [2 x i64] [i64 3, i64 0], [2 x i64] zeroinitializer], [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer, [3 x [2 x i64]] zeroinitializer], [7 x [3 x [2 x i64]]] zeroinitializer, [7 x [3 x [2 x i64]]] zeroinitializer, [7 x [3 x [2 x i64]]] zeroinitializer], align 8
@s_static = internal global i32 10, align 4
@s_global = dso_local global i32 0, align 4
@test_storage_class.sc_static = internal global i32 3, align 4
@g_arr = dso_local global [8 x i32] [i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7], align 4
@g_implicit = dso_local global [3 x i32] [i32 10, i32 20, i32 30], align 4
@tentative = dso_local global i32 20, align 4
@test_array666 = dso_local global [12 x ptr] zeroinitializer, align 8
@ptr = dso_local global [4 x [14 x ptr]] zeroinitializer, align 8
@sd = dso_local global %struct.XD zeroinitializer, align 8

define dso_local void @test_big_array() #0 {
	%2 = alloca i32, align 4
	store i32 23, ptr %2, align 4
	%3 = alloca i64, align 8
	store i64 223412, ptr %3, align 8
	%4 = alloca [11 x [7 x [3 x [2 x i64]]]], align 8
	%5 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 2, i64 0, i64 0, i64 0
	store i64 3, ptr %5, align 8
	%6 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 2, i64 0, i64 0, i64 1
	store i64 3, ptr %6, align 8
	%7 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 2, i64 0, i64 1, i64 0
	store i64 5, ptr %7, align 8
	%8 = load i32, ptr %2, align 4
	%9 = load i32, ptr %2, align 4
	%10 = mul nsw i32 %8, %9
	%11 = load i32, ptr %2, align 4
	%12 = mul nsw i32 %10, %11
	%13 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 3, i64 0, i64 0, i64 0
	%14 = sext i32 %12 to i64
	store i64 %14, ptr %13, align 8
	%15 = load i32, ptr %2, align 4
	%16 = load i32, ptr %2, align 4
	%17 = mul nsw i32 %15, %16
	%18 = load i32, ptr %2, align 4
	%19 = mul nsw i32 %17, %18
	%20 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 3, i64 0, i64 0, i64 1
	%21 = sext i32 %19 to i64
	store i64 %21, ptr %20, align 8
	%22 = load i64, ptr %3, align 8
	%23 = load i32, ptr %2, align 4
	%24 = sext i32 %23 to i64
	%25 = add nsw i64 %22, %24
	%26 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 3, i64 0, i64 1, i64 0
	store i64 %25, ptr %26, align 8
	%27 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 5, i64 0, i64 0, i64 0
	%28 = load i64, ptr %3, align 8
	store i64 %28, ptr %27, align 8
	%29 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 5, i64 0, i64 0, i64 1
	%30 = load i64, ptr %3, align 8
	store i64 %30, ptr %29, align 8
	%31 = load i64, ptr %3, align 8
	%32 = load i32, ptr %2, align 4
	%33 = sext i32 %32 to i64
	%34 = mul nsw i64 %31, %33
	%35 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 5, i64 0, i64 1, i64 0
	store i64 %34, ptr %35, align 8
	%36 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 7, i64 0, i64 0, i64 0
	store i64 1, ptr %36, align 8
	%37 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 7, i64 0, i64 0, i64 1
	store i64 1, ptr %37, align 8
	%38 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %4, i64 0, i64 7, i64 0, i64 1, i64 0
	store i64 3, ptr %38, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_basic_primitives() #0 {
	%2 = alloca i8, align 1
	store i8 65, ptr %2, align 1
	%3 = alloca i8, align 1
	store i8 123, ptr %2, align 1
	store i8 123, ptr %3, align 1
	%4 = alloca i16, align 2
	store i16 -32000, ptr %4, align 2
	%5 = alloca i32, align 4
	store i32 -2147483647, ptr %5, align 4
	%6 = alloca i64, align 8
	store i64 -2147483648, ptr %6, align 8
	%7 = alloca i64, align 8
	store i64 -9223372036854775807, ptr %7, align 8
	%8 = alloca i8, align 1
	store i8 -1, ptr %8, align 1
	%9 = alloca i16, align 2
	store i16 -1, ptr %9, align 2
	%10 = alloca i32, align 4
	store i32 -1, ptr %10, align 4
	%11 = alloca i64, align 8
	store i64 4294967295, ptr %11, align 8
	%12 = alloca i64, align 8
	store i64 -1, ptr %12, align 8
	%13 = alloca float, align 4
	store float 3.140000e+00, ptr %13, align 4
	%14 = alloca double, align 8
	store double 3.141593e+00, ptr %14, align 8
	%15 = alloca x86_fp80, align 16
	store x86_fp80 f0x4000C90FDAA22168C260, ptr %15, align 16
	%16 = alloca i8, align 1
	store i8 1, ptr %16, align 1
	%17 = alloca i8, align 1
	store i8 0, ptr %17, align 1
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_qualifiers() #0 {
	%2 = alloca i32, align 4
	store i32 42, ptr %2, align 4
	%3 = alloca i32, align 4
	store i32 0, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 99, ptr %4, align 4
	%5 = alloca ptr, align 8
	store ptr %2, ptr %5, align 8
	%6 = alloca ptr, align 8
	store ptr %2, ptr %6, align 8
	%7 = alloca ptr, align 8
	store ptr %2, ptr %7, align 8
	%8 = alloca ptr, align 8
	store ptr %3, ptr %8, align 8
	%9 = alloca ptr, align 8
	store ptr %3, ptr %9, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_storage_class() #0 {
	%2 = alloca i32, align 4
	store i32 1, ptr %2, align 4
	%3 = alloca i32, align 4
	store i32 2, ptr %3, align 4
	br label %label_1

label_1:
	ret void
}

define internal i32 @add(i32 noundef %0, i32 noundef %1) #0 {
	%3 = alloca i32, align 4
	store i32 %0, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 %1, ptr %4, align 4
	%6 = alloca i32, align 4
	%7 = load i32, ptr %3, align 4
	%8 = load i32, ptr %4, align 4
	%9 = add nsw i32 %7, %8
	store i32 %9, ptr %6, align 4
	br label %label_5

label_5:
	%10 = load i32, ptr %6, align 4
	ret i32 %10
}

define dso_local void @test_pointers() #0 {
	%2 = alloca i32, align 4
	store i32 5, ptr %2, align 4
	%3 = alloca ptr, align 8
	store ptr %2, ptr %3, align 8
	%4 = alloca ptr, align 8
	store ptr %3, ptr %4, align 8
	%5 = alloca ptr, align 8
	store ptr %4, ptr %5, align 8
	%6 = alloca ptr, align 8
	store ptr @.str.3, ptr %6, align 8
	%7 = alloca ptr, align 8
	store ptr %-1000, ptr %7, align 8
	%8 = alloca ptr, align 8
	store ptr @add, ptr %8, align 8
	%9 = alloca ptr, align 8
	%10 = alloca [4 x ptr], align 8
	%11 = alloca ptr, align 8
	store ptr null, ptr %11, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_arrays(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = alloca [4 x double], align 8
	%5 = getelementptr inbounds [4 x double], ptr %4, i64 0, i64 0
	store double 1.000000e+00, ptr %5, align 8
	%6 = getelementptr inbounds [4 x double], ptr %4, i64 0, i64 1
	store double 1.000000e+00, ptr %6, align 8
	%7 = getelementptr inbounds [4 x double], ptr %4, i64 0, i64 2
	store double 1.000000e+00, ptr %7, align 8
	%8 = getelementptr inbounds [4 x double], ptr %4, i64 0, i64 3
	store double 1.000000e+00, ptr %8, align 8
	%9 = alloca [3 x [3 x i32]], align 4
	%10 = getelementptr inbounds [3 x [3 x i32]], ptr %9, i64 0, i64 0, i64 0
	store i32 1, ptr %10, align 4
	%11 = getelementptr inbounds [3 x [3 x i32]], ptr %9, i64 0, i64 0, i64 1
	store i32 1, ptr %11, align 4
	%12 = getelementptr inbounds [3 x [3 x i32]], ptr %9, i64 0, i64 0, i64 2
	store i32 1, ptr %12, align 4
	%13 = getelementptr inbounds [3 x [3 x i32]], ptr %9, i64 0, i64 1, i64 0
	store i32 0, ptr %13, align 4
	%14 = getelementptr inbounds [3 x [3 x i32]], ptr %9, i64 0, i64 1, i64 1
	store i32 0, ptr %14, align 4
	%15 = getelementptr inbounds [3 x [3 x i32]], ptr %9, i64 0, i64 1, i64 2
	store i32 0, ptr %15, align 4
	%16 = getelementptr inbounds [3 x [3 x i32]], ptr %9, i64 0, i64 2, i64 0
	store i32 0, ptr %16, align 4
	%17 = getelementptr inbounds [3 x [3 x i32]], ptr %9, i64 0, i64 2, i64 1
	store i32 0, ptr %17, align 4
	%18 = getelementptr inbounds [3 x [3 x i32]], ptr %9, i64 0, i64 2, i64 2
	store i32 0, ptr %18, align 4
	%19 = alloca ptr, align 8
	%20 = alloca [4 x i32], align 4
	%21 = getelementptr inbounds [4 x i32], ptr %20, i64 0, i64 0
	store i32 1, ptr %21, align 4
	%22 = getelementptr inbounds [4 x i32], ptr %20, i64 0, i64 1
	store i32 1, ptr %22, align 4
	%23 = getelementptr inbounds [4 x i32], ptr %20, i64 0, i64 2
	store i32 1, ptr %23, align 4
	%24 = getelementptr inbounds [4 x i32], ptr %20, i64 0, i64 3
	store i32 1, ptr %24, align 4
	%25 = alloca [3 x ptr], align 8
	%26 = getelementptr inbounds [3 x ptr], ptr %25, i64 0, i64 0
	store ptr @.str.4, ptr %26, align 8
	%27 = getelementptr inbounds [3 x ptr], ptr %25, i64 0, i64 1
	store ptr @.str.4, ptr %27, align 8
	%28 = getelementptr inbounds [3 x ptr], ptr %25, i64 0, i64 2
	store ptr @.str.4, ptr %28, align 8
	br label %label_3

label_3:
	ret void
}

define dso_local void @test_structs() #0 {
	%2 = alloca %struct.Point, align 8
	%3 = alloca %struct.Point, align 8
	%4 = alloca %struct.Flags, align 4
	%5 = alloca %struct.Line, align 8
	%6 = alloca %union.Data, align 4
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_enums() #0 {
	%2 = alloca i32, align 4
	store i32 0, ptr %2, align 4
	%3 = alloca i32, align 4
	store i32 0, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 10, ptr %4, align 4
	br label %label_1

label_1:
	ret void
}

define dso_local void @dummy_cb(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	br label %label_3

label_3:
	ret void
}

define dso_local void @test_typedefs() #0 {
	%2 = alloca i8, align 1
	store i8 0, ptr %2, align 1
	%3 = alloca ptr, align 8
	store ptr %2, ptr %3, align 8
	%4 = alloca %struct.Point, align 8
	%5 = alloca [3 x [3 x ptr]], align 8
	%6 = getelementptr inbounds [3 x [3 x ptr]], ptr %5, i64 0, i64 0, i64 0
	%7 = getelementptr inbounds [3 x [3 x ptr]], ptr %5, i64 0, i64 0, i64 1
	%8 = getelementptr inbounds [3 x [3 x ptr]], ptr %5, i64 0, i64 0, i64 2
	%9 = getelementptr inbounds [3 x [3 x ptr]], ptr %5, i64 0, i64 1, i64 0
	%10 = getelementptr inbounds [3 x [3 x ptr]], ptr %5, i64 0, i64 1, i64 1
	%11 = getelementptr inbounds [3 x [3 x ptr]], ptr %5, i64 0, i64 1, i64 2
	%12 = getelementptr inbounds [3 x [3 x ptr]], ptr %5, i64 0, i64 2, i64 0
	%13 = getelementptr inbounds [3 x [3 x ptr]], ptr %5, i64 0, i64 2, i64 1
	%14 = getelementptr inbounds [3 x [3 x ptr]], ptr %5, i64 0, i64 2, i64 2
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_compound_literals() #0 {
	%2 = alloca ptr, align 8
	%3 = alloca i32, align 4
	%4 = alloca ptr, align 8
	%5 = alloca %struct.Point, align 8
	%6 = alloca ptr, align 8
	%7 = alloca [4 x i32], align 4
	%8 = getelementptr inbounds [4 x i32], ptr %7, i64 0, i64 0
	store i32 10, ptr %8, align 4
	%9 = getelementptr inbounds [4 x i32], ptr %7, i64 0, i64 1
	store i32 10, ptr %9, align 4
	%10 = getelementptr inbounds [4 x i32], ptr %7, i64 0, i64 2
	store i32 10, ptr %10, align 4
	%11 = getelementptr inbounds [4 x i32], ptr %7, i64 0, i64 3
	store i32 10, ptr %11, align 4
	store ptr %7, ptr %6, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_multi_declarator() #0 {
	%2 = alloca i32, align 4
	store i32 1, ptr %2, align 4
	%3 = alloca i32, align 4
	store i32 2, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 3, ptr %4, align 4
	%5 = alloca ptr, align 8
	store ptr %2, ptr %5, align 8
	%6 = alloca ptr, align 8
	store ptr %3, ptr %6, align 8
	%7 = alloca i32, align 4
	store i32 0, ptr %7, align 4
	%8 = alloca double, align 8
	store double 1.000000e+00, ptr %8, align 8
	%9 = alloca double, align 8
	store double 2.000000e+00, ptr %9, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_for_init_decl() #0 {
	%4 = alloca i32, align 4
	store i32 0, ptr %4, align 4
	br label %label_5

label_5:
	%6 = load i32, ptr %4, align 4
	%7 = icmp slt i32 %6, 10
	%8 = icmp ne i1 %7, 0
	br i1 %8, label %label_2, label %label_3

label_2:
	br label %label_5

label_3:
	%9 = alloca i32, align 4
	%10 = alloca i32, align 4
	store i32 0, ptr %9, align 4
	store i32 10, ptr %10, align 4
	br label %label_13

label_13:
	%14 = load i32, ptr %9, align 4
	%15 = load i32, ptr %10, align 4
	%16 = icmp slt i32 %14, %15
	%17 = icmp ne i1 %16, 0
	br i1 %17, label %label_11, label %label_12

label_11:
	br label %label_13

label_12:
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_mixed_decl_code() #0 {
	%2 = alloca i32, align 4
	store i32 10, ptr %2, align 4
	%3 = load i32, ptr %2, align 4
	%4 = add nsw i32 %3, 5
	store i32 %4, ptr %2, align 4
	%5 = alloca i32, align 4
	%6 = load i32, ptr %2, align 4
	%7 = mul nsw i32 %6, 2
	store i32 %7, ptr %5, align 4
	%8 = alloca double, align 8
	store double 3.140000e+00, ptr %8, align 8
	%9 = load double, ptr %8, align 8
	%10 = fmul double %9, 2.000000
	store double %10, ptr %8, align 8
	%11 = alloca i64, align 8
	%12 = load double, ptr %8, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @arr_param_decay(ptr noundef %0, i32 noundef %1) #0 {
	%3 = alloca [0 x i32], align 4
	store ptr %0, ptr %3, align 8
	%4 = alloca i32, align 4
	store i32 %1, ptr %4, align 4
	br label %label_5

label_5:
	ret void
}

define dso_local void @test_edge_cases() #0 {
	%2 = alloca i32, align 4
	store i32 5, ptr %2, align 4
	%3 = alloca i8, align 1
	store i8 1, ptr %3, align 1
	%4 = alloca i32, align 4
	store i32 0, ptr %4, align 4
	%5 = alloca ptr, align 8
	store ptr %4, ptr %5, align 8
	%6 = alloca i32, align 4
	store i32 0, ptr %6, align 4
	%7 = alloca ptr, align 8
	store ptr %6, ptr %7, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local i32 @main() #0 {
	%2 = alloca i32, align 4
	call void @test_basic_primitives()
	call void @test_qualifiers()
	call void @test_storage_class()
	call void @test_pointers()
	call void @test_arrays(i32 noundef 6)
	call void @test_structs()
	call void @test_enums()
	call void @test_typedefs()
	call void @test_compound_literals()
	call void @test_multi_declarator()
	call void @test_for_init_decl()
	call void @test_mixed_decl_code()
	call void @test_edge_cases()
	store i32 0, ptr %2, align 4
	br label %label_1

label_1:
	%3 = load i32, ptr %2, align 4
	ret i32 %3
}


attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}