%struct.XD = type { [4 x [2 x i32]] }
%struct.Point = type { double, double }

@.str.1 = private unnamed_addr constant [3 x i8] c"xd\00", align 1
@.str.2 = private unnamed_addr constant [6 x i8] c"pt\09ro\00", align 1
@.str.3 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@.str.4 = private unnamed_addr constant [4 x i8] c"foo\00", align 1
@.str.5 = private unnamed_addr constant [4 x i8] c"bar\00", align 1
@.str.6 = private unnamed_addr constant [4 x i8] c"baz\00", align 1
@word = dso_local global [2 x ptr] [ptr@.str.1, ptr@.str.2], align 8
@test_array333 = dso_local global [3 x [4 x float]] [[4 x float] [float 60000.000000, float 3.000000, float 0.000000e+00, float 0.000000e+00], [4 x float] zeroinitializer, [4 x float] [float 4.000000, float 2.000000, float 0.000000e+00, float 0.000000e+00]], align 4
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
	%1 = alloca i32, align 4
	%2 = alloca i64, align 8
	%3 = alloca [11 x [7 x [3 x [2 x i64]]]], align 8
	store i32 23, ptr %1, align 4
	store i64 223412, ptr %2, align 8
	call void @llvm.memset.p0.i64(ptr align 8 %3, i8 0, i64 3696, i1 false)
	%4 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 2, i64 0, i64 0, i64 0
	store i64 3, ptr %4, align 8
	%5 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 2, i64 0, i64 0, i64 1
	store i64 4, ptr %5, align 8
	%6 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 2, i64 0, i64 1, i64 0
	store i64 5, ptr %6, align 8
	%7 = load i32, ptr %1, align 4
	%8 = load i32, ptr %1, align 4
	%9 = mul nsw i32 %7, %8
	%10 = load i32, ptr %1, align 4
	%11 = mul nsw i32 %9, %10
	%12 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 3, i64 0, i64 0, i64 0
	%13 = sext i32 %11 to i64
	store i64 %13, ptr %12, align 8
	%14 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 3, i64 0, i64 0, i64 1
	store i64 1000, ptr %14, align 8
	%15 = load i64, ptr %2, align 8
	%16 = load i32, ptr %1, align 4
	%17 = sext i32 %16 to i64
	%18 = add nsw i64 %15, %17
	%19 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 3, i64 0, i64 1, i64 0
	store i64 %18, ptr %19, align 8
	%20 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 5, i64 0, i64 0, i64 0
	%21 = load i64, ptr %2, align 8
	store i64 %21, ptr %20, align 8
	%22 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 5, i64 0, i64 0, i64 1
	store i64 2, ptr %22, align 8
	%23 = load i64, ptr %2, align 8
	%24 = load i32, ptr %1, align 4
	%25 = sext i32 %24 to i64
	%26 = mul nsw i64 %23, %25
	%27 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 5, i64 0, i64 1, i64 0
	store i64 %26, ptr %27, align 8
	%28 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 7, i64 0, i64 0, i64 0
	store i64 1, ptr %28, align 8
	%29 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 7, i64 0, i64 0, i64 1
	store i64 2, ptr %29, align 8
	%30 = getelementptr inbounds [11 x [7 x [3 x [2 x i64]]]], ptr %3, i64 0, i64 7, i64 0, i64 1, i64 0
	store i64 3, ptr %30, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_basic_primitives() #0 {
	%1 = alloca i8, align 1
	%2 = alloca i8, align 1
	%3 = alloca i16, align 2
	%4 = alloca i32, align 4
	%5 = alloca i64, align 8
	%6 = alloca i64, align 8
	%7 = alloca i8, align 1
	%8 = alloca i16, align 2
	%9 = alloca i32, align 4
	%10 = alloca i64, align 8
	%11 = alloca i64, align 8
	%12 = alloca float, align 4
	%13 = alloca double, align 8
	%14 = alloca x86_fp80, align 16
	%15 = alloca i8, align 1
	%16 = alloca i8, align 1
	store i8 65, ptr %1, align 1
	store i8 123, ptr %1, align 1
	store i8 123, ptr %2, align 1
	store i16 -32000, ptr %3, align 2
	store i32 -2147483647, ptr %4, align 4
	store i64 -2147483648, ptr %5, align 8
	store i64 -9223372036854775807, ptr %6, align 8
	store i8 -1, ptr %7, align 1
	store i16 -1, ptr %8, align 2
	store i32 -1, ptr %9, align 4
	store i64 4294967295, ptr %10, align 8
	store i64 -1, ptr %11, align 8
	store float 0x40091EB860000000, ptr %12, align 4
	store double 0x400921FB54442D18, ptr %13, align 8
	store x86_fp80 0xK4000C90FDAA22168C260, ptr %14, align 16
	store i8 1, ptr %15, align 1
	store i8 0, ptr %16, align 1
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_qualifiers() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca ptr, align 8
	%5 = alloca ptr, align 8
	%6 = alloca ptr, align 8
	%7 = alloca ptr, align 8
	%8 = alloca ptr, align 8
	store i32 42, ptr %1, align 4
	store i32 0, ptr %2, align 4
	store i32 99, ptr %3, align 4
	store ptr %1, ptr %4, align 8
	store ptr %1, ptr %5, align 8
	store ptr %1, ptr %6, align 8
	store ptr %2, ptr %7, align 8
	store ptr %2, ptr %8, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_storage_class() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	store i32 1, ptr %1, align 4
	store i32 2, ptr %2, align 4
	br label %label_1

label_1:
	ret void
}

define internal i32 @add(i32 noundef %0, i32 noundef %1) #0 {
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	store i32 %0, ptr %3, align 4
	store i32 %1, ptr %4, align 4
	%6 = load i32, ptr %3, align 4
	%7 = load i32, ptr %4, align 4
	%8 = add nsw i32 %6, %7
	store i32 %8, ptr %5, align 4
	br label %label_5

label_5:
	%9 = load i32, ptr %5, align 4
	ret i32 %9
}

define dso_local void @test_pointers() #0 {
	%1 = alloca i32, align 4
	%2 = alloca ptr, align 8
	%3 = alloca ptr, align 8
	%4 = alloca ptr, align 8
	%5 = alloca ptr, align 8
	%6 = alloca ptr, align 8
	%7 = alloca ptr, align 8
	%8 = alloca ptr, align 8
	%9 = alloca [4 x ptr], align 8
	%10 = alloca ptr, align 8
	store i32 5, ptr %1, align 4
	store ptr %1, ptr %2, align 8
	store ptr %2, ptr %3, align 8
	store ptr %3, ptr %4, align 8
	store ptr @.str.3, ptr %5, align 8
	store ptr null, ptr %6, align 8
	store ptr @add, ptr %7, align 8
	store ptr null, ptr %10, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_arrays(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca [4 x double], align 8
	%4 = alloca [3 x [3 x i32]], align 4
	%5 = alloca [3 x ptr], align 8
	store i32 %0, ptr %2, align 4
	call void @llvm.memset.p0.i64(ptr align 8 %3, i8 0, i64 32, i1 false)
	%6 = getelementptr inbounds [4 x double], ptr %3, i64 0, i64 0
	store double 0x3FF0000000000000, ptr %6, align 8
	%7 = getelementptr inbounds [4 x double], ptr %3, i64 0, i64 1
	store double 0x4000000000000000, ptr %7, align 8
	%8 = getelementptr inbounds [4 x double], ptr %3, i64 0, i64 2
	store double 0x4008000000000000, ptr %8, align 8
	%9 = getelementptr inbounds [4 x double], ptr %3, i64 0, i64 3
	store double 0x4010000000000000, ptr %9, align 8
	call void @llvm.memset.p0.i64(ptr align 4 %4, i8 0, i64 36, i1 false)
	%10 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 0, i64 0
	store i32 1, ptr %10, align 4
	%11 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 0, i64 1
	store i32 0, ptr %11, align 4
	%12 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 0, i64 2
	store i32 0, ptr %12, align 4
	%13 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 1, i64 0
	store i32 0, ptr %13, align 4
	%14 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 1, i64 1
	store i32 1, ptr %14, align 4
	%15 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 1, i64 2
	store i32 0, ptr %15, align 4
	%16 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 2, i64 0
	store i32 0, ptr %16, align 4
	%17 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 2, i64 1
	store i32 0, ptr %17, align 4
	%18 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 2, i64 2
	store i32 1, ptr %18, align 4
	call void @llvm.memset.p0.i64(ptr align 1 %5, i8 0, i64 24, i1 false)
	%19 = getelementptr inbounds [3 x ptr], ptr %5, i64 0, i64 0
	store ptr @.str.4, ptr %19, align 8
	%20 = getelementptr inbounds [3 x ptr], ptr %5, i64 0, i64 1
	store ptr @.str.5, ptr %20, align 8
	%21 = getelementptr inbounds [3 x ptr], ptr %5, i64 0, i64 2
	store ptr @.str.6, ptr %21, align 8
	br label %label_3

label_3:
	ret void
}

define dso_local void @test_enums() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 0, ptr %1, align 4
	store i32 0, ptr %2, align 4
	store i32 10, ptr %3, align 4
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
	%1 = alloca i8, align 1
	%2 = alloca ptr, align 8
	%3 = alloca %struct.Point, align 8
	%4 = alloca [3 x [3 x i32]], align 8
	%5 = alloca [4 x i32], align 8
	%6 = alloca ptr, align 8
	%7 = alloca ptr, align 8
	%8 = alloca [4 x [5 x i32]], align 8
	%9 = alloca ptr, align 8
	store i8 -85, ptr %1, align 1
	store ptr %1, ptr %2, align 8
	call void @llvm.memset.p0.i64(ptr align 8 %3, i8 0, i64 16, i1 false)
	%10 = getelementptr inbounds nuw %struct.Point, ptr %3, i32 0, i32 0
	store double 0x4008000000000000, ptr %10, align 8
	%11 = getelementptr inbounds nuw %struct.Point, ptr %3, i32 0, i32 1
	store double 0x4010000000000000, ptr %11, align 8
	call void @llvm.memset.p0.i64(ptr align 8 %4, i8 0, i64 72, i1 false)
	%12 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 0, i64 0
	store i32 1, ptr %12, align 4
	%13 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 0, i64 1
	store i32 0, ptr %13, align 4
	%14 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 0, i64 2
	store i32 0, ptr %14, align 4
	%15 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 1, i64 0
	store i32 0, ptr %15, align 4
	%16 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 1, i64 1
	store i32 1, ptr %16, align 4
	%17 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 1, i64 2
	store i32 0, ptr %17, align 4
	%18 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 2, i64 0
	store i32 0, ptr %18, align 4
	%19 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 2, i64 1
	store i32 0, ptr %19, align 4
	%20 = getelementptr inbounds [3 x [3 x i32]], ptr %4, i64 0, i64 2, i64 2
	store i32 1, ptr %20, align 4
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_compound_literals() #0 {
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_multi_declarator() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca ptr, align 8
	%5 = alloca ptr, align 8
	%6 = alloca i32, align 4
	%7 = alloca double, align 8
	%8 = alloca double, align 8
	store i32 1, ptr %1, align 4
	store i32 2, ptr %2, align 4
	store i32 3, ptr %3, align 4
	store ptr %1, ptr %4, align 8
	store ptr %2, ptr %5, align 8
	store i32 0, ptr %6, align 4
	store double 0x3FF0000000000000, ptr %7, align 8
	store double 0x4000000000000000, ptr %8, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_for_init_decl() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 0, ptr %1, align 4
	br label %label_6

label_6:
	%4 = load i32, ptr %1, align 4
	%5 = icmp slt i32 %4, 10
	%6 = icmp ne i1 %5, 0
	br i1 %6, label %label_2, label %label_3

label_2:
	br label %label_4

label_4:
	%7 = load i32, ptr %1, align 4
	%8 = add nsw i32 %7, 1
	store i32 %8, ptr %1, align 4
	br label %label_6

label_3:
	store i32 0, ptr %2, align 4
	store i32 10, ptr %3, align 4
	br label %label_17

label_17:
	%9 = load i32, ptr %2, align 4
	%10 = load i32, ptr %3, align 4
	%11 = icmp slt i32 %9, %10
	%12 = icmp ne i1 %11, 0
	br i1 %12, label %label_14, label %label_15

label_14:
	br label %label_16

label_16:
	%13 = load i32, ptr %2, align 4
	%14 = add nsw i32 %13, 1
	store i32 %14, ptr %2, align 4
	%15 = load i32, ptr %3, align 4
	%16 = sub nsw i32 %15, 1
	store i32 %16, ptr %3, align 4
	br label %label_17

label_15:
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_mixed_decl_code() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca double, align 8
	%4 = alloca i64, align 8
	store i32 10, ptr %1, align 4
	%5 = load i32, ptr %1, align 4
	%6 = add nsw i32 %5, 5
	store i32 %6, ptr %1, align 4
	%7 = load i32, ptr %1, align 4
	%8 = mul nsw i32 %7, 2
	store i32 %8, ptr %2, align 4
	store double 0x40091EB851EB851F, ptr %3, align 8
	%9 = load double, ptr %3, align 8
	%10 = fmul double %9, 2.000000
	store double %10, ptr %3, align 8
	%11 = load double, ptr %3, align 8
	%12 = fptosi double %11 to i64
	store i64 %12, ptr %4, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @arr_param_decay(ptr noundef %0, i32 noundef %1) #0 {
	%3 = alloca ptr, align 8
	%4 = alloca i32, align 4
	store ptr %0, ptr %3, align 8
	store i32 %1, ptr %4, align 4
	br label %label_5

label_5:
	ret void
}

define dso_local void @test_edge_cases() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i8, align 1
	%3 = alloca i32, align 4
	%4 = alloca ptr, align 8
	%5 = alloca i32, align 4
	%6 = alloca ptr, align 8
	store i32 5, ptr %1, align 4
	store i8 1, ptr %2, align 1
	store i32 57005, ptr %3, align 4
	store ptr %3, ptr %4, align 8
	store i32 0, ptr %5, align 4
	store ptr %5, ptr %6, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local void @test_array_decay() #0 {
	%1 = alloca [4 x i32], align 4
	%2 = alloca ptr, align 8
	call void @llvm.memset.p0.i64(ptr align 4 %1, i8 0, i64 16, i1 false)
	%3 = getelementptr inbounds [4 x i32], ptr %1, i64 0, i64 0
	store i32 1, ptr %3, align 4
	%4 = getelementptr inbounds [4 x i32], ptr %1, i64 0, i64 1
	store i32 2, ptr %4, align 4
	%5 = getelementptr inbounds [4 x i32], ptr %1, i64 0, i64 2
	store i32 3, ptr %5, align 4
	%6 = getelementptr inbounds [4 x i32], ptr %1, i64 0, i64 3
	store i32 4, ptr %6, align 4
	%7 = getelementptr inbounds [4 x i32], ptr %1, i64 0, i64 0
	store ptr %7, ptr %2, align 8
	br label %label_1

label_1:
	ret void
}

define dso_local i32 @main() #0 {
	%1 = alloca i32, align 4
	store i32 0, ptr %1, align 4
	call void @test_basic_primitives()
	call void @test_qualifiers()
	call void @test_storage_class()
	call void @test_pointers()
	call void @test_arrays(i32 noundef 6)
	call void @test_enums()
	call void @test_typedefs()
	call void @test_compound_literals()
	call void @test_multi_declarator()
	call void @test_for_init_decl()
	call void @test_mixed_decl_code()
	call void @test_edge_cases()
	store i32 0, ptr %1, align 4
	br label %label_1

label_1:
	%2 = load i32, ptr %1, align 4
	ret i32 %2
}

declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }