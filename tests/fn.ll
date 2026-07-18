%struct._IO_FILE = type { i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i32, i64, i16, i8, [1 x i8], ptr, i64, ptr, ptr, ptr, ptr, i64, i32, [20 x i8] }
%struct.Pair = type { i32, i32, i32, i16 }
%struct.Pair3 = type { i32, i32, i32, i16, i64, double }
%struct.Pair2 = type { i32, i32, i32 }

@.str.1 = private unnamed_addr constant [20 x i8] c"FAIL: %s (line %d)\0A\00", align 1
@.str.2 = private unnamed_addr constant [10 x i8] c"f0()==123\00", align 1
@.str.3 = private unnamed_addr constant [11 x i8] c"f1(41)==42\00", align 1
@.str.4 = private unnamed_addr constant [14 x i8] c"f2(20,22)==42\00", align 1
@.str.5 = private unnamed_addr constant [18 x i8] c"f5(1,2,3,4,5)==15\00", align 1
@.str.6 = private unnamed_addr constant [36 x i8] c"many_args(1,2,3,4,5,6,7,8,9,10)==55\00", align 1
@.str.7 = private unnamed_addr constant [21 x i8] c"sum_array(arr,5)==15\00", align 1
@.str.8 = private unnamed_addr constant [16 x i8] c"factorial(1)==1\00", align 1
@.str.9 = private unnamed_addr constant [18 x i8] c"factorial(5)==120\00", align 1
@.str.10 = private unnamed_addr constant [27 x i8] c"call_binary(add,10,20)==30\00", align 1
@.str.11 = private unnamed_addr constant [28 x i8] c"call_binary(sub,10,20)==-10\00", align 1
@.str.12 = private unnamed_addr constant [8 x i8] c"p.x==17\00", align 1
@.str.13 = private unnamed_addr constant [8 x i8] c"p.y==25\00", align 1
@.str.14 = private unnamed_addr constant [16 x i8] c"pair_sum(p)==42\00", align 1
@.str.15 = private unnamed_addr constant [19 x i8] c"mixed(1,2,3,4)==10\00", align 1
@.str.16 = private unnamed_addr constant [19 x i8] c"inc(triple(7))==22\00", align 1
@.str.17 = private unnamed_addr constant [10 x i8] c"bump()==1\00", align 1
@.str.18 = private unnamed_addr constant [10 x i8] c"bump()==2\00", align 1
@.str.19 = private unnamed_addr constant [10 x i8] c"bump()==3\00", align 1
@.str.20 = private unnamed_addr constant [22 x i8] c"f2(f1(10),f1(20))==32\00", align 1
@.str.21 = private unnamed_addr constant [6 x i8] c"PASS\0A\00", align 1
@.str.22 = private unnamed_addr constant [19 x i8] c"%d test(s) failed\0A\00", align 1
@failures = internal global i32 1, align 4
@counter = internal global i32 0, align 4
@stderr = external global ptr
@stdout = external global ptr
@stdin = external global ptr

define dso_local i32 @foo(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	%6 = load i32, ptr %2, align 4
	%7 = mul nsw i32 %6, 2
	store i32 %7, ptr %5, align 4
	%8 = load i32, ptr %5, align 4
	%9 = add nsw i32 %8, 5
	store i32 %9, ptr %5, align 4
	%10 = load i32, ptr %5, align 4
	%11 = add nsw i32 %10, 1
	store i32 %11, ptr %4, align 4
	br label %label_3

label_3:
	%12 = load i32, ptr %4, align 4
	ret i32 %12
}

define dso_local void @voidRet() #0 {
	br label %label_1

label_1:
	ret void
}

define dso_local i8 @f_neg() #0 {
	%2 = alloca i8, align 1
	%3 = alloca i32, align 4
	store i32 2, ptr %3, align 4
	%4 = trunc i32 %-20000 to i8
	store i8 %-20000, ptr %2, align 1
	br label %label_1

label_1:
	%5 = load i8, ptr %2, align 1
	ret i8 %5
}

define dso_local i32 @f0() #0 {
	%2 = alloca i32, align 4
	store i32 123, ptr %2, align 4
	br label %label_1

label_1:
	%3 = load i32, ptr %2, align 4
	ret i32 %3
}

define dso_local i32 @f1(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = alloca i32, align 4
	%5 = load i32, ptr %2, align 4
	%6 = add nsw i32 %5, 1
	store i32 %6, ptr %4, align 4
	br label %label_3

label_3:
	%7 = load i32, ptr %4, align 4
	ret i32 %7
}

define dso_local i32 @f_ptr(ptr noundef %0) #0 {
	%2 = alloca ptr, align 8
	store ptr %0, ptr %2, align 8
	%4 = alloca i32, align 4
	%5 = add void %0, %0
	store i32 %5, ptr %4, align 4
	br label %label_3

label_3:
	%6 = load i32, ptr %4, align 4
	ret i32 %6
}

define dso_local i32 @f2(i32 noundef %0, i32 noundef %1) #0 {
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

define dso_local i32 @f5(i32 noundef %0, i32 noundef %1, i32 noundef %2, i32 noundef %3, i32 noundef %4) #0 {
	%6 = alloca i32, align 4
	store i32 %0, ptr %6, align 4
	%7 = alloca i32, align 4
	store i32 %1, ptr %7, align 4
	%8 = alloca i32, align 4
	store i32 %2, ptr %8, align 4
	%9 = alloca i32, align 4
	store i32 %3, ptr %9, align 4
	%10 = alloca i32, align 4
	store i32 %4, ptr %10, align 4
	%12 = alloca i32, align 4
	%13 = load i32, ptr %6, align 4
	%14 = load i32, ptr %7, align 4
	%15 = add nsw i32 %13, %14
	%16 = load i32, ptr %8, align 4
	%17 = add nsw i32 %15, %16
	%18 = load i32, ptr %9, align 4
	%19 = add nsw i32 %17, %18
	%20 = load i32, ptr %10, align 4
	%21 = add nsw i32 %19, %20
	store i32 %21, ptr %12, align 4
	br label %label_11

label_11:
	%22 = load i32, ptr %12, align 4
	ret i32 %22
}

define dso_local i64 @many_args(i64 noundef %0, i64 noundef %1, i64 noundef %2, i64 noundef %3, i64 noundef %4, i64 noundef %5, i64 noundef %6, i64 noundef %7, i64 noundef %8, i64 noundef %9) #0 {
	%11 = alloca i64, align 8
	store i64 %0, ptr %11, align 8
	%12 = alloca i64, align 8
	store i64 %1, ptr %12, align 8
	%13 = alloca i64, align 8
	store i64 %2, ptr %13, align 8
	%14 = alloca i64, align 8
	store i64 %3, ptr %14, align 8
	%15 = alloca i64, align 8
	store i64 %4, ptr %15, align 8
	%16 = alloca i64, align 8
	store i64 %5, ptr %16, align 8
	%17 = alloca i64, align 8
	store i64 %6, ptr %17, align 8
	%18 = alloca i64, align 8
	store i64 %7, ptr %18, align 8
	%19 = alloca i64, align 8
	store i64 %8, ptr %19, align 8
	%20 = alloca i64, align 8
	store i64 %9, ptr %20, align 8
	%22 = alloca i64, align 8
	%23 = load i64, ptr %11, align 8
	%24 = load i64, ptr %12, align 8
	%25 = add nsw i64 %23, %24
	%26 = load i64, ptr %13, align 8
	%27 = add nsw i64 %25, %26
	%28 = load i64, ptr %14, align 8
	%29 = add nsw i64 %27, %28
	%30 = load i64, ptr %15, align 8
	%31 = add nsw i64 %29, %30
	%32 = load i64, ptr %16, align 8
	%33 = add nsw i64 %31, %32
	%34 = load i64, ptr %17, align 8
	%35 = add nsw i64 %33, %34
	%36 = load i64, ptr %18, align 8
	%37 = add nsw i64 %35, %36
	%38 = load i64, ptr %19, align 8
	%39 = add nsw i64 %37, %38
	%40 = load i64, ptr %20, align 8
	%41 = add nsw i64 %39, %40
	store i64 %41, ptr %22, align 8
	br label %label_21

label_21:
	%42 = load i64, ptr %22, align 8
	ret i64 %42
}

define dso_local i32 @sum_array(ptr noundef %0, i32 noundef %1) #0 {
	%3 = alloca ptr, align 8
	store ptr %0, ptr %3, align 8
	%4 = alloca i32, align 4
	store i32 %1, ptr %4, align 4
	%6 = alloca i32, align 4
	%7 = alloca i32, align 4
	store i32 0, ptr %7, align 4
	%8 = alloca i32, align 4
	store i32 0, ptr %8, align 4
	br label %label_11

label_11:
	%12 = load i32, ptr %8, align 4
	%13 = load i32, ptr %4, align 4
	%14 = icmp slt i32 %12, %13
	%15 = icmp ne i1 %14, 0
	br i1 %15, label %label_9, label %label_10

label_9:
	%16 = load i32, ptr %7, align 4
	%17 = add void %0, %0
	br label %label_11

label_10:
	store i32 %-20000, ptr %6, align 4
	br label %label_5

label_5:
	%18 = load i32, ptr %6, align 4
	ret i32 %18
}

define dso_local i32 @factorial(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = alloca i32, align 4
	%7 = load i32, ptr %2, align 4
	%8 = icmp sle i32 %7, 1
	%9 = icmp ne i1 %8, 0
	br i1 %9, label %label_6, label %label_5

label_6:
	store i32 1, ptr %4, align 4
	br label %label_3

label_5:
	%10 = load i32, ptr %2, align 4
	%11 = load i32, ptr %2, align 4
	%12 = sub nsw i32 %11, 1
	%13 = call i32 @factorial(i32 noundef %12)
	%14 = mul nsw i32 %10, %13
	store i32 %14, ptr %4, align 4
	br label %label_3

label_3:
	%15 = load i32, ptr %4, align 4
	ret i32 %15
}

define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 {
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

define dso_local i32 @sub(i32 noundef %0, i32 noundef %1) #0 {
	%3 = alloca i32, align 4
	store i32 %0, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 %1, ptr %4, align 4
	%6 = alloca i32, align 4
	%7 = load i32, ptr %3, align 4
	%8 = load i32, ptr %4, align 4
	%9 = sub nsw i32 %7, %8
	store i32 %9, ptr %6, align 4
	br label %label_5

label_5:
	%10 = load i32, ptr %6, align 4
	ret i32 %10
}

define dso_local i32 @call_binary(ptr noundef %0, i32 noundef %1, i32 noundef %2) #0 {
	%4 = alloca ptr, align 8
	store ptr %0, ptr %4, align 8
	%5 = alloca i32, align 4
	store i32 %1, ptr %5, align 4
	%6 = alloca i32, align 4
	store i32 %2, ptr %6, align 4
	%8 = alloca i32, align 4
	store i32 0, ptr %8, align 4
	br label %label_7

label_7:
	%9 = load i32, ptr %8, align 4
	ret i32 %9
}

define dso_local { i64, i64 } @make_pair(i32 noundef %0, i32 noundef %1, i64 noundef %2, i32 noundef %3, i64 noundef %4, i64 noundef %5) #0 {
	%7 = alloca i32, align 4
	store i32 %0, ptr %7, align 4
	%8 = alloca i32, align 4
	store i32 %1, ptr %8, align 4
	%9 = alloca %struct.Pair2, align 4
	%10 = alloca { i64, i32 }, align 4
	%11 = getelementptr inbounds nuw { i64, i32 }, ptr %10, i32 0, i32 0
	store i64 %2, ptr %11, align 4
	%12 = getelementptr inbounds nuw { i64, i32 }, ptr %10, i32 0, i32 1
	store i32 %3, ptr %12, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %9, ptr align 4 %10, i64 12, i1 false)
	%13 = alloca %struct.Pair, align 4
	%14 = alloca { i64, i64 }, align 4
	%15 = getelementptr inbounds nuw { i64, i64 }, ptr %14, i32 0, i32 0
	store i64 %4, ptr %15, align 4
	%16 = getelementptr inbounds nuw { i64, i64 }, ptr %14, i32 0, i32 1
	store i64 %5, ptr %16, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %13, ptr align 4 %14, i64 16, i1 false)
	%18 = alloca %struct.Pair, align 4 
	%21 = load i32, ptr %7, align 4
	%22 = srem i32 %21, 2
	%23 = icmp eq i32 %22, 0
	%24 = icmp ne i1 %23, 0
	br i1 %24, label %label_20, label %label_19

label_20:
	%25 = alloca %struct.Pair, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %18, ptr align 4 %25, i64 16, i1 false)
	br label %label_17

label_19:
	%26 = alloca %struct.Pair, align 4
	%27 = getelementptr inbounds %struct.Pair, ptr %26, i64 0, i64 0
	%28 = load i32, ptr %7, align 4
	store i32 %28, ptr %27, align 4
	%29 = getelementptr inbounds %struct.Pair, ptr %13, i64 0, i64 0
	%30 = getelementptr inbounds %struct.Pair, ptr %26, i64 0, i64 1
	store i32 %29, ptr %30, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %18, ptr align 4 %26, i64 16, i1 false)
	br label %label_17

label_17:
	%31 = load { i64, i64 }, ptr %18, align 4
	ret { i64, i64 } %31
}

define dso_local void @make_pair2(ptr dead_on_unwind noalias writable sret(%struct.Pair3) align 8 %0, i32 noundef %1, i32 noundef %2, i64 noundef %3, i32 noundef %4, ptr noundef byval(%struct.Pair) align 8 %7) #0 {
	%9 = alloca i32, align 4
	store i32 %1, ptr %9, align 4
	%10 = alloca i32, align 4
	store i32 %2, ptr %10, align 4
	%11 = alloca %struct.Pair2, align 4
	%12 = alloca { i64, i32 }, align 4
	%13 = getelementptr inbounds nuw { i64, i32 }, ptr %12, i32 0, i32 0
	store i64 %3, ptr %13, align 4
	%14 = getelementptr inbounds nuw { i64, i32 }, ptr %12, i32 0, i32 1
	store i32 %4, ptr %14, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %11, ptr align 4 %12, i64 12, i1 false)
	%18 = load i32, ptr %9, align 4
	%19 = srem i32 %18, 2
	%20 = icmp eq i32 %19, 0
	%21 = icmp ne i1 %20, 0
	br i1 %21, label %label_17, label %label_16

label_17:
	%22 = alloca %struct.Pair3, align 8
	call void @llvm.memcpy.p0.p0.i64(ptr align 8 %0, ptr align 8 %22, i64 32, i1 false)
	br label %label_15

label_16:
	%23 = alloca %struct.Pair3, align 8
	%24 = getelementptr inbounds %struct.Pair3, ptr %23, i64 0, i64 0
	%25 = load i32, ptr %9, align 4
	store i32 %25, ptr %24, align 4
	%26 = getelementptr inbounds %struct.Pair, ptr %7, i64 0, i64 0
	%27 = getelementptr inbounds %struct.Pair3, ptr %23, i64 0, i64 1
	store i32 %26, ptr %27, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 8 %0, ptr align 8 %23, i64 32, i1 false)
	br label %label_15

label_15:
	ret void
}

define dso_local i32 @pair_sum(i64 noundef %0, i64 noundef %1) #0 {
	%3 = alloca %struct.Pair, align 4
	%4 = alloca { i64, i64 }, align 4
	%5 = getelementptr inbounds nuw { i64, i64 }, ptr %4, i32 0, i32 0
	store i64 %0, ptr %5, align 4
	%6 = getelementptr inbounds nuw { i64, i64 }, ptr %4, i32 0, i32 1
	store i64 %1, ptr %6, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %3, ptr align 4 %4, i64 16, i1 false)
	%8 = alloca i32, align 4
	%9 = getelementptr inbounds %struct.Pair, ptr %3, i64 0, i64 0
	%10 = getelementptr inbounds %struct.Pair, ptr %3, i64 0, i64 2
	%11 = add nsw i32 %9, %10
	store i32 %11, ptr %8, align 4
	br label %label_7

label_7:
	%12 = load i32, ptr %8, align 4
	ret i32 %12
}

define dso_local i64 @mixed(i8 noundef %0, i16 noundef %1, i32 noundef %2, i64 noundef %3) #0 {
	%5 = alloca i8, align 1
	store i8 %0, ptr %5, align 1
	%6 = alloca i16, align 2
	store i16 %1, ptr %6, align 2
	%7 = alloca i32, align 4
	store i32 %2, ptr %7, align 4
	%8 = alloca i64, align 8
	store i64 %3, ptr %8, align 8
	%10 = alloca i64, align 8
	%11 = load i8, ptr %5, align 1
	%12 = load i16, ptr %6, align 2
	%13 = sext i8 %11 to i32
	%14 = sext i16 %12 to i32
	%15 = add nsw i32 %13, %14
	%16 = load i32, ptr %7, align 4
	%17 = add nsw i32 %15, %16
	%18 = load i64, ptr %8, align 8
	%19 = sext i32 %17 to i64
	%20 = add nsw i64 %19, %18
	store i64 %20, ptr %10, align 8
	br label %label_9

label_9:
	%21 = load i64, ptr %10, align 8
	ret i64 %21
}

define dso_local i32 @inc(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = alloca i32, align 4
	%5 = load i32, ptr %2, align 4
	%6 = add nsw i32 %5, 1
	store i32 %6, ptr %4, align 4
	br label %label_3

label_3:
	%7 = load i32, ptr %4, align 4
	ret i32 %7
}

define dso_local i32 @triple(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = alloca i32, align 4
	%5 = load i32, ptr %2, align 4
	%6 = mul nsw i32 %5, 3
	store i32 %6, ptr %4, align 4
	br label %label_3

label_3:
	%7 = load i32, ptr %4, align 4
	ret i32 %7
}

define dso_local i32 @bump() #0 {
	%2 = alloca i32, align 4
	store i32 %-20000, ptr %2, align 4
	br label %label_1

label_1:
	%3 = load i32, ptr %2, align 4
	ret i32 %3
}

define dso_local i32 @main() #0 {
	%2 = alloca i32, align 4
	%3 = alloca [5 x i32], align 4
	%4 = alloca %struct.Pair, align 4
	br label %label_6

label_6:
	%9 = call i32 @f0()
	%10 = icmp eq i32 %9, 123
	%11 = xor i1 %10, 1
	%12 = icmp ne i1 %11, 0
	br i1 %12, label %label_8, label %label_7

label_8:
	%13 = call i32 (ptr, ...) @printf(@.str.1, @.str.2, i32 noundef 236)
	br label %label_7

label_7:
	br label %label_5

label_5:
	br label %label_15

label_15:
	%18 = call i32 @f1(i32 noundef 41)
	%19 = icmp eq i32 %18, 42
	%20 = xor i1 %19, 1
	%21 = icmp ne i1 %20, 0
	br i1 %21, label %label_17, label %label_16

label_17:
	%22 = call i32 (ptr, ...) @printf(@.str.1, @.str.3, i32 noundef 237)
	br label %label_16

label_16:
	br label %label_14

label_14:
	br label %label_24

label_24:
	%27 = call i32 @f2(i32 noundef 20, i32 noundef 22)
	%28 = icmp eq i32 %27, 42
	%29 = xor i1 %28, 1
	%30 = icmp ne i1 %29, 0
	br i1 %30, label %label_26, label %label_25

label_26:
	%31 = call i32 (ptr, ...) @printf(@.str.1, @.str.4, i32 noundef 238)
	br label %label_25

label_25:
	br label %label_23

label_23:
	br label %label_33

label_33:
	%36 = call i32 @f5(i32 noundef 1, i32 noundef 2, i32 noundef 3, i32 noundef 4, i32 noundef 5)
	%37 = icmp eq i32 %36, 15
	%38 = xor i1 %37, 1
	%39 = icmp ne i1 %38, 0
	br i1 %39, label %label_35, label %label_34

label_35:
	%40 = call i32 (ptr, ...) @printf(@.str.1, @.str.5, i32 noundef 240)
	br label %label_34

label_34:
	br label %label_32

label_32:
	br label %label_42

label_42:
	%45 = call i64 @many_args(i64 noundef 1, i64 noundef 2, i64 noundef 3, i64 noundef 4, i64 noundef 5, i64 noundef 6, i64 noundef 7, i64 noundef 8, i64 noundef 9, i64 noundef 10)
	%46 = icmp eq i64 %45, 55
	%47 = xor i1 %46, 1
	%48 = icmp ne i1 %47, 0
	br i1 %48, label %label_44, label %label_43

label_44:
	%49 = call i32 (ptr, ...) @printf(@.str.1, @.str.6, i32 noundef 245)
	br label %label_43

label_43:
	br label %label_41

label_41:
	br label %label_51

label_51:
	%54 = getelementptr inbounds [5 x i32], ptr %3, i64 0, i64 0
	%55 = call i32 @sum_array(ptr noundef %54, i32 noundef 5)
	%56 = icmp eq i32 %55, 15
	%57 = xor i1 %56, 1
	%58 = icmp ne i1 %57, 0
	br i1 %58, label %label_53, label %label_52

label_53:
	%59 = call i32 (ptr, ...) @printf(@.str.1, @.str.7, i32 noundef 253)
	br label %label_52

label_52:
	br label %label_50

label_50:
	br label %label_61

label_61:
	%64 = call i32 @factorial(i32 noundef 1)
	%65 = icmp eq i32 %64, 1
	%66 = xor i1 %65, 1
	%67 = icmp ne i1 %66, 0
	br i1 %67, label %label_63, label %label_62

label_63:
	%68 = call i32 (ptr, ...) @printf(@.str.1, @.str.8, i32 noundef 255)
	br label %label_62

label_62:
	br label %label_60

label_60:
	br label %label_70

label_70:
	%73 = call i32 @factorial(i32 noundef 5)
	%74 = icmp eq i32 %73, 120
	%75 = xor i1 %74, 1
	%76 = icmp ne i1 %75, 0
	br i1 %76, label %label_72, label %label_71

label_72:
	%77 = call i32 (ptr, ...) @printf(@.str.1, @.str.9, i32 noundef 256)
	br label %label_71

label_71:
	br label %label_69

label_69:
	br label %label_79

label_79:
	store ptr @add, ptr %82, align 8
	%83 = load ptr, ptr %82, align 8
	%84 = call i32 @call_binary(ptr noundef %83, i32 noundef 10, i32 noundef 20)
	%85 = icmp eq i32 %84, 30
	%86 = xor i1 %85, 1
	%87 = icmp ne i1 %86, 0
	br i1 %87, label %label_81, label %label_80

label_81:
	%88 = call i32 (ptr, ...) @printf(@.str.1, @.str.10, i32 noundef 258)
	br label %label_80

label_80:
	br label %label_78

label_78:
	br label %label_90

label_90:
	store ptr @sub, ptr %93, align 8
	%94 = load ptr, ptr %93, align 8
	%95 = call i32 @call_binary(ptr noundef %94, i32 noundef 10, i32 noundef 20)
	%96 = icmp eq i32 %95, -10
	%97 = xor i1 %96, 1
	%98 = icmp ne i1 %97, 0
	br i1 %98, label %label_92, label %label_91

label_92:
	%99 = call i32 (ptr, ...) @printf(@.str.1, @.str.11, i32 noundef 259)
	br label %label_91

label_91:
	br label %label_89

label_89:
	%100 = alloca %struct.Pair2, align 4
	%101 = getelementptr inbounds { i64, i32 }, ptr %100, i64 0, i64 0
	%102 = load i64, ptr %101, align 4
	%103 = getelementptr inbounds { i64, i32 }, ptr %100, i64 0, i64 1
	%104 = load i32, ptr %103, align 4
	%105 = getelementptr inbounds { i64, i64 }, ptr %4, i64 0, i64 0
	%106 = load i64, ptr %105, align 4
	%107 = getelementptr inbounds { i64, i64 }, ptr %4, i64 0, i64 1
	%108 = load i64, ptr %107, align 4
	%109 = call { i64, i64 } @make_pair(i32 noundef 17, i32 noundef 25, i64 noundef %102, i32 noundef %104, i64 noundef %106, i64 noundef %108)
	%110 = alloca %struct.Pair3, align 8
	%111 = getelementptr inbounds { i64, i32 }, ptr %100, i64 0, i64 0
	%112 = load i64, ptr %111, align 4
	%113 = getelementptr inbounds { i64, i32 }, ptr %100, i64 0, i64 1
	%114 = load i32, ptr %113, align 4
	%115 = alloca %struct.Pair, align 4 
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %115, ptr align 4 %4, i64 16, i1 false)
	%116 = alloca %struct.Pair3, align 8 
	call void @make_pair2(ptr dead_on_unwind noalias writable sret(%struct.Pair3) align 8 %116, i32 noundef 17, i32 noundef 25, i64 noundef %112, i32 noundef %114, ptr noundef byval(%struct.Pair) align 8 %115)
	br label %label_118

label_118:
	%121 = getelementptr inbounds %struct.Pair, ptr %4, i64 0, i64 0
	%122 = icmp eq i32 %121, 17
	%123 = xor i1 %122, 1
	%124 = icmp ne i1 %123, 0
	br i1 %124, label %label_120, label %label_119

label_120:
	%125 = call i32 (ptr, ...) @printf(@.str.1, @.str.12, i32 noundef 265)
	br label %label_119

label_119:
	br label %label_117

label_117:
	br label %label_127

label_127:
	%130 = getelementptr inbounds %struct.Pair, ptr %4, i64 0, i64 2
	%131 = icmp eq i32 %130, 25
	%132 = xor i1 %131, 1
	%133 = icmp ne i1 %132, 0
	br i1 %133, label %label_129, label %label_128

label_129:
	%134 = call i32 (ptr, ...) @printf(@.str.1, @.str.13, i32 noundef 266)
	br label %label_128

label_128:
	br label %label_126

label_126:
	br label %label_136

label_136:
	%139 = getelementptr inbounds { i64, i64 }, ptr %4, i64 0, i64 0
	%140 = load i64, ptr %139, align 4
	%141 = getelementptr inbounds { i64, i64 }, ptr %4, i64 0, i64 1
	%142 = load i64, ptr %141, align 4
	%143 = call i32 @pair_sum(i64 noundef %140, i64 noundef %142)
	%144 = icmp eq i32 %143, 42
	%145 = xor i1 %144, 1
	%146 = icmp ne i1 %145, 0
	br i1 %146, label %label_138, label %label_137

label_138:
	%147 = call i32 (ptr, ...) @printf(@.str.1, @.str.14, i32 noundef 267)
	br label %label_137

label_137:
	br label %label_135

label_135:
	br label %label_149

label_149:
	%152 = call i64 @mixed(i8 noundef 1, i16 noundef 2, i32 noundef 3, i64 noundef 4)
	%153 = icmp eq i64 %152, 10
	%154 = xor i1 %153, 1
	%155 = icmp ne i1 %154, 0
	br i1 %155, label %label_151, label %label_150

label_151:
	%156 = call i32 (ptr, ...) @printf(@.str.1, @.str.15, i32 noundef 269)
	br label %label_150

label_150:
	br label %label_148

label_148:
	br label %label_158

label_158:
	%161 = call i32 @triple(i32 noundef 7)
	%162 = call i32 @inc(i32 noundef %161)
	%163 = icmp eq i32 %162, 22
	%164 = xor i1 %163, 1
	%165 = icmp ne i1 %164, 0
	br i1 %165, label %label_160, label %label_159

label_160:
	%166 = call i32 (ptr, ...) @printf(@.str.1, @.str.16, i32 noundef 271)
	br label %label_159

label_159:
	br label %label_157

label_157:
	br label %label_168

label_168:
	%171 = call i32 @bump()
	%172 = icmp eq i32 %171, 1
	%173 = xor i1 %172, 1
	%174 = icmp ne i1 %173, 0
	br i1 %174, label %label_170, label %label_169

label_170:
	%175 = call i32 (ptr, ...) @printf(@.str.1, @.str.17, i32 noundef 273)
	br label %label_169

label_169:
	br label %label_167

label_167:
	br label %label_177

label_177:
	%180 = call i32 @bump()
	%181 = icmp eq i32 %180, 2
	%182 = xor i1 %181, 1
	%183 = icmp ne i1 %182, 0
	br i1 %183, label %label_179, label %label_178

label_179:
	%184 = call i32 (ptr, ...) @printf(@.str.1, @.str.18, i32 noundef 274)
	br label %label_178

label_178:
	br label %label_176

label_176:
	br label %label_186

label_186:
	%189 = call i32 @bump()
	%190 = icmp eq i32 %189, 3
	%191 = xor i1 %190, 1
	%192 = icmp ne i1 %191, 0
	br i1 %192, label %label_188, label %label_187

label_188:
	%193 = call i32 (ptr, ...) @printf(@.str.1, @.str.19, i32 noundef 275)
	br label %label_187

label_187:
	br label %label_185

label_185:
	br label %label_195

label_195:
	%198 = call i32 @f1(i32 noundef 10)
	%199 = call i32 @f1(i32 noundef 20)
	%200 = call i32 @f2(i32 noundef %198, i32 noundef %199)
	%201 = icmp eq i32 %200, 32
	%202 = xor i1 %201, 1
	%203 = icmp ne i1 %202, 0
	br i1 %203, label %label_197, label %label_196

label_197:
	%204 = call i32 (ptr, ...) @printf(@.str.1, @.str.20, i32 noundef 277)
	br label %label_196

label_196:
	br label %label_194

label_194:
	%207 = load i32, ptr @failures, align 4
	%208 = icmp eq i32 %207, 0
	%209 = icmp ne i1 %208, 0
	br i1 %209, label %label_206, label %label_205

label_206:
	%210 = call i32 (ptr, ...) @printf(@.str.21)
	store i32 0, ptr %2, align 4
	br label %label_1

label_205:
	%211 = load i32, ptr @failures, align 4
	%212 = call i32 (ptr, ...) @printf(@.str.22, i32 noundef %211)
	store i32 1, ptr %2, align 4
	br label %label_1

label_1:
	%213 = load i32, ptr %2, align 4
	ret i32 %213
}

declare i32 @printf(ptr noundef %0, ...) #1
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}
attributes #2 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }