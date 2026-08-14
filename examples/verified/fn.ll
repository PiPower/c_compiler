%struct._IO_FILE = type { i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i32, i64, i16, i8, [1 x i8], ptr, i64, ptr, ptr, ptr, ptr, i64, i32, [20 x i8] }
%struct.Pair = type { i32, i32, i32, i16 }
%struct.Pair2 = type { i32, i32, i32 }
%struct.Pair3 = type { i32, i32, i32, i16, i64, double }

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
@failures = internal global i32 0, align 4
@counter = internal global i32 0, align 4
@stderr = external global ptr
@stdout = external global ptr
@stdin = external global ptr

define dso_local i32 @foo(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%5 = load i32, ptr %2, align 4
	%6 = mul nsw i32 %5, 2
	store i32 %6, ptr %4, align 4
	%7 = load i32, ptr %4, align 4
	%8 = add nsw i32 %7, 5
	store i32 %8, ptr %4, align 4
	%9 = load i32, ptr %4, align 4
	%10 = add nsw i32 %9, 1
	store i32 %10, ptr %3, align 4
	br label %label_3

label_3:
	%11 = load i32, ptr %3, align 4
	ret i32 %11
}

define dso_local void @voidRet() #0 {
	br label %label_1

label_1:
	ret void
}

define dso_local i8 @f_neg() #0 {
	%1 = alloca i8, align 1
	%2 = alloca i32, align 4
	store i32 2, ptr %2, align 4
	%3 = load i32, ptr %2, align 4
	%4 = trunc i32 %3 to i8
	store i8 %4, ptr %1, align 1
	br label %label_1

label_1:
	%5 = load i8, ptr %1, align 1
	ret i8 %5
}

define dso_local i32 @f0() #0 {
	%1 = alloca i32, align 4
	store i32 123, ptr %1, align 4
	br label %label_1

label_1:
	%2 = load i32, ptr %1, align 4
	ret i32 %2
}

define dso_local i32 @f1(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = load i32, ptr %2, align 4
	%5 = add nsw i32 %4, 1
	store i32 %5, ptr %3, align 4
	br label %label_3

label_3:
	%6 = load i32, ptr %3, align 4
	ret i32 %6
}

define dso_local i32 @f_ptr(ptr noundef %0) #0 {
	%2 = alloca ptr, align 8
	%3 = alloca i32, align 4
	store ptr %0, ptr %2, align 8
	%4 = load ptr, ptr %2, align 8
	%5 = load i32, ptr %4, align 4
	%6 = add nsw i32 %5, 34
	store i32 %6, ptr %3, align 4
	br label %label_3

label_3:
	%7 = load i32, ptr %3, align 4
	ret i32 %7
}

define dso_local i32 @f2(i32 noundef %0, i32 noundef %1) #0 {
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

define dso_local i32 @f5(i32 noundef %0, i32 noundef %1, i32 noundef %2, i32 noundef %3, i32 noundef %4) #0 {
	%6 = alloca i32, align 4
	%7 = alloca i32, align 4
	%8 = alloca i32, align 4
	%9 = alloca i32, align 4
	%10 = alloca i32, align 4
	%11 = alloca i32, align 4
	store i32 %0, ptr %6, align 4
	store i32 %1, ptr %7, align 4
	store i32 %2, ptr %8, align 4
	store i32 %3, ptr %9, align 4
	store i32 %4, ptr %10, align 4
	%12 = load i32, ptr %6, align 4
	%13 = load i32, ptr %7, align 4
	%14 = add nsw i32 %12, %13
	%15 = load i32, ptr %8, align 4
	%16 = add nsw i32 %14, %15
	%17 = load i32, ptr %9, align 4
	%18 = add nsw i32 %16, %17
	%19 = load i32, ptr %10, align 4
	%20 = add nsw i32 %18, %19
	store i32 %20, ptr %11, align 4
	br label %label_11

label_11:
	%21 = load i32, ptr %11, align 4
	ret i32 %21
}

define dso_local i64 @many_args(i64 noundef %0, i64 noundef %1, i64 noundef %2, i64 noundef %3, i64 noundef %4, i64 noundef %5, i64 noundef %6, i64 noundef %7, i64 noundef %8, i64 noundef %9) #0 {
	%11 = alloca i64, align 8
	%12 = alloca i64, align 8
	%13 = alloca i64, align 8
	%14 = alloca i64, align 8
	%15 = alloca i64, align 8
	%16 = alloca i64, align 8
	%17 = alloca i64, align 8
	%18 = alloca i64, align 8
	%19 = alloca i64, align 8
	%20 = alloca i64, align 8
	%21 = alloca i64, align 8
	store i64 %0, ptr %11, align 8
	store i64 %1, ptr %12, align 8
	store i64 %2, ptr %13, align 8
	store i64 %3, ptr %14, align 8
	store i64 %4, ptr %15, align 8
	store i64 %5, ptr %16, align 8
	store i64 %6, ptr %17, align 8
	store i64 %7, ptr %18, align 8
	store i64 %8, ptr %19, align 8
	store i64 %9, ptr %20, align 8
	%22 = load i64, ptr %11, align 8
	%23 = load i64, ptr %12, align 8
	%24 = add nsw i64 %22, %23
	%25 = load i64, ptr %13, align 8
	%26 = add nsw i64 %24, %25
	%27 = load i64, ptr %14, align 8
	%28 = add nsw i64 %26, %27
	%29 = load i64, ptr %15, align 8
	%30 = add nsw i64 %28, %29
	%31 = load i64, ptr %16, align 8
	%32 = add nsw i64 %30, %31
	%33 = load i64, ptr %17, align 8
	%34 = add nsw i64 %32, %33
	%35 = load i64, ptr %18, align 8
	%36 = add nsw i64 %34, %35
	%37 = load i64, ptr %19, align 8
	%38 = add nsw i64 %36, %37
	%39 = load i64, ptr %20, align 8
	%40 = add nsw i64 %38, %39
	store i64 %40, ptr %21, align 8
	br label %label_21

label_21:
	%41 = load i64, ptr %21, align 8
	ret i64 %41
}

define dso_local i32 @sum_array(ptr noundef %0, i32 noundef %1) #0 {
	%3 = alloca ptr, align 8
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	%6 = alloca i32, align 4
	%7 = alloca i32, align 4
	store ptr %0, ptr %3, align 8
	store i32 %1, ptr %4, align 4
	store i32 0, ptr %6, align 4
	store i32 0, ptr %7, align 4
	br label %label_11

label_11:
	%8 = load i32, ptr %7, align 4
	%9 = load i32, ptr %4, align 4
	%10 = icmp slt i32 %8, %9
	%11 = icmp ne i1 %10, 0
	br i1 %11, label %label_9, label %label_10

label_9:
	%12 = load i32, ptr %6, align 4
	%13 = load ptr, ptr %3, align 8
	%14 = load i32, ptr %7, align 4
	%15 = sext i32 %14 to i64
	%16 = getelementptr inbounds i32, ptr %13, i64 %15
	%17 = load i32, ptr %16, align 4
	%18 = add nsw i32 %12, %17
	store i32 %18, ptr %6, align 4
	%19 = load i32, ptr %7, align 4
	%20 = add nsw i32 %19, 1
	store i32 %20, ptr %7, align 4
	br label %label_11

label_10:
	%21 = load i32, ptr %6, align 4
	store i32 %21, ptr %5, align 4
	br label %label_5

label_5:
	%22 = load i32, ptr %5, align 4
	ret i32 %22
}

define dso_local i32 @factorial(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = load i32, ptr %2, align 4
	%5 = icmp sle i32 %4, 1
	%6 = icmp ne i1 %5, 0
	br i1 %6, label %label_6, label %label_5

label_6:
	store i32 1, ptr %3, align 4
	br label %label_3

label_5:
	%7 = load i32, ptr %2, align 4
	%8 = load i32, ptr %2, align 4
	%9 = sub nsw i32 %8, 1
	%10 = call i32 @factorial(i32 noundef %9)
	%11 = mul nsw i32 %7, %10
	store i32 %11, ptr %3, align 4
	br label %label_3

label_3:
	%12 = load i32, ptr %3, align 4
	ret i32 %12
}

define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 {
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

define dso_local i32 @sub(i32 noundef %0, i32 noundef %1) #0 {
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	store i32 %0, ptr %3, align 4
	store i32 %1, ptr %4, align 4
	%6 = load i32, ptr %3, align 4
	%7 = load i32, ptr %4, align 4
	%8 = sub nsw i32 %6, %7
	store i32 %8, ptr %5, align 4
	br label %label_5

label_5:
	%9 = load i32, ptr %5, align 4
	ret i32 %9
}

define dso_local i32 @call_binary(ptr noundef %0, i32 noundef %1, i32 noundef %2) #0 {
	%4 = alloca ptr, align 8
	%5 = alloca i32, align 4
	%6 = alloca i32, align 4
	%7 = alloca i32, align 4
	store ptr %0, ptr %4, align 8
	store i32 %1, ptr %5, align 4
	store i32 %2, ptr %6, align 4
	store i32 0, ptr %7, align 4
	br label %label_7

label_7:
	%8 = load i32, ptr %7, align 4
	ret i32 %8
}

define dso_local { i64, i64 } @make_pair(i32 noundef %0, i32 noundef %1, i64 noundef %2, i32 noundef %3, i64 noundef %4, i64 noundef %5) #0 {
	%7 = alloca i32, align 4
	%8 = alloca i32, align 4
	%9 = alloca %struct.Pair2, align 4
	%10 = alloca { i64, i32 }, align 8
	%11 = alloca %struct.Pair, align 4
	%12 = alloca { i64, i64 }, align 8
	%13 = alloca %struct.Pair, align 4 
	%14 = alloca %struct.Pair, align 4
	%15 = alloca %struct.Pair, align 4
	store i32 %0, ptr %7, align 4
	store i32 %1, ptr %8, align 4
	%16 = getelementptr inbounds nuw { i64, i32 }, ptr %10, i32 0, i32 0
	store i64 %2, ptr %16, align 4
	%17 = getelementptr inbounds nuw { i64, i32 }, ptr %10, i32 0, i32 1
	store i32 %3, ptr %17, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %9, ptr align 4 %10, i64 12, i1 false)
	%18 = getelementptr inbounds nuw { i64, i64 }, ptr %12, i32 0, i32 0
	store i64 %4, ptr %18, align 4
	%19 = getelementptr inbounds nuw { i64, i64 }, ptr %12, i32 0, i32 1
	store i64 %5, ptr %19, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %11, ptr align 4 %12, i64 16, i1 false)
	%20 = load i32, ptr %7, align 4
	%21 = srem i32 %20, 2
	%22 = icmp eq i32 %21, 0
	%23 = icmp ne i1 %22, 0
	br i1 %23, label %label_20, label %label_19

label_20:
	call void @llvm.memset.p0.i64(ptr align 4 %14, i8 0, i64 16, i1 false)
	%24 = getelementptr inbounds nuw %struct.Pair, ptr %14, i32 0, i32 0
	store i32 2, ptr %24, align 4
	%25 = getelementptr inbounds nuw %struct.Pair, ptr %14, i32 0, i32 1
	store i32 4, ptr %25, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %13, ptr align 4 %14, i64 16, i1 false)
	br label %label_17

label_19:
	%26 = getelementptr inbounds nuw %struct.Pair, ptr %15, i32 0, i32 0
	%27 = load i32, ptr %7, align 4
	store i32 %27, ptr %26, align 4
	%28 = getelementptr inbounds nuw %struct.Pair, ptr %11, i32 0, i32 0
	%29 = getelementptr inbounds nuw %struct.Pair, ptr %15, i32 0, i32 1
	%30 = load i32, ptr %28, align 4
	store i32 %30, ptr %29, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %13, ptr align 4 %15, i64 16, i1 false)
	br label %label_17

label_17:
	%31 = load { i64, i64 }, ptr %13, align 4
	ret { i64, i64 } %31
}

define dso_local void @make_pair2(ptr dead_on_unwind noalias writable sret(%struct.Pair3) align 8 %0, i32 noundef %1, i32 noundef %2, i64 noundef %3, i32 noundef %4, ptr noundef byval(%struct.Pair) align 4 %7) #0 {
	%9 = alloca i32, align 4
	%10 = alloca i32, align 4
	%11 = alloca %struct.Pair2, align 4
	%12 = alloca { i64, i32 }, align 8
	%13 = alloca %struct.Pair3, align 8
	%14 = alloca %struct.Pair3, align 8
	store i32 %1, ptr %9, align 4
	store i32 %2, ptr %10, align 4
	%15 = getelementptr inbounds nuw { i64, i32 }, ptr %12, i32 0, i32 0
	store i64 %3, ptr %15, align 4
	%16 = getelementptr inbounds nuw { i64, i32 }, ptr %12, i32 0, i32 1
	store i32 %4, ptr %16, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %11, ptr align 4 %12, i64 12, i1 false)
	%17 = load i32, ptr %9, align 4
	%18 = srem i32 %17, 2
	%19 = icmp eq i32 %18, 0
	%20 = icmp ne i1 %19, 0
	br i1 %20, label %label_17, label %label_16

label_17:
	call void @llvm.memset.p0.i64(ptr align 8 %13, i8 0, i64 32, i1 false)
	%21 = getelementptr inbounds nuw %struct.Pair3, ptr %13, i32 0, i32 0
	store i32 2, ptr %21, align 4
	%22 = getelementptr inbounds nuw %struct.Pair3, ptr %13, i32 0, i32 1
	store i32 4, ptr %22, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 8 %0, ptr align 8 %13, i64 32, i1 false)
	br label %label_15

label_16:
	%23 = getelementptr inbounds nuw %struct.Pair3, ptr %14, i32 0, i32 0
	%24 = load i32, ptr %9, align 4
	store i32 %24, ptr %23, align 4
	%25 = getelementptr inbounds nuw %struct.Pair, ptr %7, i32 0, i32 0
	%26 = getelementptr inbounds nuw %struct.Pair3, ptr %14, i32 0, i32 1
	%27 = load i32, ptr %25, align 4
	store i32 %27, ptr %26, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 8 %0, ptr align 8 %14, i64 32, i1 false)
	br label %label_15

label_15:
	ret void
}

define dso_local i32 @pair_sum(i64 noundef %0, i64 noundef %1) #0 {
	%3 = alloca %struct.Pair, align 4
	%4 = alloca { i64, i64 }, align 8
	%5 = alloca i32, align 4
	%6 = getelementptr inbounds nuw { i64, i64 }, ptr %4, i32 0, i32 0
	store i64 %0, ptr %6, align 4
	%7 = getelementptr inbounds nuw { i64, i64 }, ptr %4, i32 0, i32 1
	store i64 %1, ptr %7, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %3, ptr align 4 %4, i64 16, i1 false)
	%8 = getelementptr inbounds nuw %struct.Pair, ptr %3, i32 0, i32 0
	%9 = load i32, ptr %8, align 4
	%10 = getelementptr inbounds nuw %struct.Pair, ptr %3, i32 0, i32 2
	%11 = load i32, ptr %10, align 4
	%12 = add nsw i32 %9, %11
	store i32 %12, ptr %5, align 4
	br label %label_7

label_7:
	%13 = load i32, ptr %5, align 4
	ret i32 %13
}

define dso_local i64 @mixed(i8 noundef %0, i16 noundef %1, i32 noundef %2, i64 noundef %3) #0 {
	%5 = alloca i8, align 1
	%6 = alloca i16, align 2
	%7 = alloca i32, align 4
	%8 = alloca i64, align 8
	%9 = alloca i64, align 8
	store i8 %0, ptr %5, align 1
	store i16 %1, ptr %6, align 2
	store i32 %2, ptr %7, align 4
	store i64 %3, ptr %8, align 8
	%10 = load i8, ptr %5, align 1
	%11 = sext i8 %10 to i64
	%12 = load i16, ptr %6, align 2
	%13 = sext i16 %12 to i64
	%14 = add nsw i64 %11, %13
	%15 = load i32, ptr %7, align 4
	%16 = sext i32 %15 to i64
	%17 = add nsw i64 %14, %16
	%18 = load i64, ptr %8, align 8
	%19 = add nsw i64 %17, %18
	store i64 %19, ptr %9, align 8
	br label %label_9

label_9:
	%20 = load i64, ptr %9, align 8
	ret i64 %20
}

define dso_local i32 @inc(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = load i32, ptr %2, align 4
	%5 = add nsw i32 %4, 1
	store i32 %5, ptr %3, align 4
	br label %label_3

label_3:
	%6 = load i32, ptr %3, align 4
	ret i32 %6
}

define dso_local i32 @triple(i32 noundef %0) #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%4 = load i32, ptr %2, align 4
	%5 = mul nsw i32 %4, 3
	store i32 %5, ptr %3, align 4
	br label %label_3

label_3:
	%6 = load i32, ptr %3, align 4
	ret i32 %6
}

define dso_local i32 @bump() #0 {
	%1 = alloca i32, align 4
	%2 = load i32, ptr @counter, align 4
	%3 = add nsw i32 %2, 1
	store i32 %3, ptr @counter, align 4
	%4 = load i32, ptr @counter, align 4
	store i32 %4, ptr %1, align 4
	br label %label_1

label_1:
	%5 = load i32, ptr %1, align 4
	ret i32 %5
}

define dso_local i32 @main() #0 {
	%1 = alloca i32, align 4
	%2 = alloca [5 x i32], align 4
	%3 = alloca %struct.Pair, align 4
	%4 = alloca %struct.Pair2, align 4
	%5 = alloca { i64, i64 }, align 8
	%6 = alloca %struct.Pair3, align 8
	%7 = alloca %struct.Pair, align 4 
	%8 = alloca %struct.Pair3, align 8 
	store i32 0, ptr %1, align 4
	call void @llvm.memset.p0.i64(ptr align 4 %3, i8 0, i64 16, i1 false)
	br label %label_6

label_6:
	%9 = call i32 @f0()
	%10 = icmp eq i32 %9, 123
	%11 = xor i1 %10, 1
	%12 = icmp ne i1 %11, 0
	br i1 %12, label %label_8, label %label_7

label_8:
	%13 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.2, i32 noundef 235)
	%14 = load i32, ptr @failures, align 4
	%15 = add nsw i32 %14, 1
	store i32 %15, ptr @failures, align 4
	br label %label_7

label_7:
	br label %label_5

label_5:
	br label %label_17

label_17:
	%16 = call i32 @f1(i32 noundef 41)
	%17 = icmp eq i32 %16, 42
	%18 = xor i1 %17, 1
	%19 = icmp ne i1 %18, 0
	br i1 %19, label %label_19, label %label_18

label_19:
	%20 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.3, i32 noundef 236)
	%21 = load i32, ptr @failures, align 4
	%22 = add nsw i32 %21, 1
	store i32 %22, ptr @failures, align 4
	br label %label_18

label_18:
	br label %label_16

label_16:
	br label %label_28

label_28:
	%23 = call i32 @f2(i32 noundef 20, i32 noundef 22)
	%24 = icmp eq i32 %23, 42
	%25 = xor i1 %24, 1
	%26 = icmp ne i1 %25, 0
	br i1 %26, label %label_30, label %label_29

label_30:
	%27 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.4, i32 noundef 237)
	%28 = load i32, ptr @failures, align 4
	%29 = add nsw i32 %28, 1
	store i32 %29, ptr @failures, align 4
	br label %label_29

label_29:
	br label %label_27

label_27:
	br label %label_39

label_39:
	%30 = call i32 @f5(i32 noundef 1, i32 noundef 2, i32 noundef 3, i32 noundef 4, i32 noundef 5)
	%31 = icmp eq i32 %30, 15
	%32 = xor i1 %31, 1
	%33 = icmp ne i1 %32, 0
	br i1 %33, label %label_41, label %label_40

label_41:
	%34 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.5, i32 noundef 239)
	%35 = load i32, ptr @failures, align 4
	%36 = add nsw i32 %35, 1
	store i32 %36, ptr @failures, align 4
	br label %label_40

label_40:
	br label %label_38

label_38:
	br label %label_50

label_50:
	%37 = call i64 @many_args(i64 noundef 1, i64 noundef 2, i64 noundef 3, i64 noundef 4, i64 noundef 5, i64 noundef 6, i64 noundef 7, i64 noundef 8, i64 noundef 9, i64 noundef 10)
	%38 = icmp eq i64 %37, 55
	%39 = xor i1 %38, 1
	%40 = icmp ne i1 %39, 0
	br i1 %40, label %label_52, label %label_51

label_52:
	%41 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.6, i32 noundef 244)
	%42 = load i32, ptr @failures, align 4
	%43 = add nsw i32 %42, 1
	store i32 %43, ptr @failures, align 4
	br label %label_51

label_51:
	br label %label_49

label_49:
	%44 = getelementptr inbounds [5 x i32], ptr %2, i64 0, i64 0
	%45 = getelementptr inbounds i32, ptr %44, i64 0
	store i32 1, ptr %45, align 4
	%46 = getelementptr inbounds [5 x i32], ptr %2, i64 0, i64 0
	%47 = getelementptr inbounds i32, ptr %46, i64 1
	store i32 2, ptr %47, align 4
	%48 = getelementptr inbounds [5 x i32], ptr %2, i64 0, i64 0
	%49 = getelementptr inbounds i32, ptr %48, i64 2
	store i32 3, ptr %49, align 4
	%50 = getelementptr inbounds [5 x i32], ptr %2, i64 0, i64 0
	%51 = getelementptr inbounds i32, ptr %50, i64 3
	store i32 4, ptr %51, align 4
	%52 = getelementptr inbounds [5 x i32], ptr %2, i64 0, i64 0
	%53 = getelementptr inbounds i32, ptr %52, i64 4
	store i32 5, ptr %53, align 4
	br label %label_71

label_71:
	%54 = getelementptr inbounds [5 x i32], ptr %2, i64 0, i64 0
	%55 = call i32 @sum_array(ptr noundef %54, i32 noundef 5)
	%56 = icmp eq i32 %55, 15
	%57 = xor i1 %56, 1
	%58 = icmp ne i1 %57, 0
	br i1 %58, label %label_73, label %label_72

label_73:
	%59 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.7, i32 noundef 252)
	%60 = load i32, ptr @failures, align 4
	%61 = add nsw i32 %60, 1
	store i32 %61, ptr @failures, align 4
	br label %label_72

label_72:
	br label %label_70

label_70:
	br label %label_83

label_83:
	%62 = call i32 @factorial(i32 noundef 1)
	%63 = icmp eq i32 %62, 1
	%64 = xor i1 %63, 1
	%65 = icmp ne i1 %64, 0
	br i1 %65, label %label_85, label %label_84

label_85:
	%66 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.8, i32 noundef 254)
	%67 = load i32, ptr @failures, align 4
	%68 = add nsw i32 %67, 1
	store i32 %68, ptr @failures, align 4
	br label %label_84

label_84:
	br label %label_82

label_82:
	br label %label_94

label_94:
	%69 = call i32 @factorial(i32 noundef 5)
	%70 = icmp eq i32 %69, 120
	%71 = xor i1 %70, 1
	%72 = icmp ne i1 %71, 0
	br i1 %72, label %label_96, label %label_95

label_96:
	%73 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.9, i32 noundef 255)
	%74 = load i32, ptr @failures, align 4
	%75 = add nsw i32 %74, 1
	store i32 %75, ptr @failures, align 4
	br label %label_95

label_95:
	br label %label_93

label_93:
	br label %label_105

label_105:
	%76 = call i32 @call_binary(ptr noundef @add, i32 noundef 10, i32 noundef 20)
	%77 = icmp eq i32 %76, 30
	%78 = xor i1 %77, 1
	%79 = icmp ne i1 %78, 0
	br i1 %79, label %label_107, label %label_106

label_107:
	%80 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.10, i32 noundef 257)
	%81 = load i32, ptr @failures, align 4
	%82 = add nsw i32 %81, 1
	store i32 %82, ptr @failures, align 4
	br label %label_106

label_106:
	br label %label_104

label_104:
	br label %label_116

label_116:
	%83 = call i32 @call_binary(ptr noundef @sub, i32 noundef 10, i32 noundef 20)
	%84 = icmp eq i32 %83, -10
	%85 = xor i1 %84, 1
	%86 = icmp ne i1 %85, 0
	br i1 %86, label %label_118, label %label_117

label_118:
	%87 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.11, i32 noundef 258)
	%88 = load i32, ptr @failures, align 4
	%89 = add nsw i32 %88, 1
	store i32 %89, ptr @failures, align 4
	br label %label_117

label_117:
	br label %label_115

label_115:
	call void @llvm.memset.p0.i64(ptr align 4 %4, i8 0, i64 12, i1 false)
	%90 = getelementptr inbounds { i64, i32 }, ptr %4, i32 0, i32 0
	%91 = load i64, ptr %90, align 4
	%92 = getelementptr inbounds { i64, i32 }, ptr %4, i32 0, i32 1
	%93 = load i32, ptr %92, align 4
	%94 = getelementptr inbounds { i64, i64 }, ptr %3, i32 0, i32 0
	%95 = load i64, ptr %94, align 4
	%96 = getelementptr inbounds { i64, i64 }, ptr %3, i32 0, i32 1
	%97 = load i64, ptr %96, align 4
	%98 = call { i64, i64 } @make_pair(i32 noundef 17, i32 noundef 25, i64 noundef %91, i32 noundef %93, i64 noundef %95, i64 noundef %97)
	%99 = getelementptr inbounds { i64, i64 }, ptr %5, i32 0, i32 0
	%100 = extractvalue { i64, i64 } %98, 0
	store i64 %100, ptr %99, align 4
	%101 = getelementptr inbounds { i64, i64 }, ptr %5, i32 0, i32 1
	%102 = extractvalue { i64, i64 } %98, 1
	store i64 %102, ptr %101, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %3, ptr align 4 %5, i64 16, i1 false)
	call void @llvm.memset.p0.i64(ptr align 8 %6, i8 0, i64 32, i1 false)
	%103 = getelementptr inbounds { i64, i32 }, ptr %4, i32 0, i32 0
	%104 = load i64, ptr %103, align 4
	%105 = getelementptr inbounds { i64, i32 }, ptr %4, i32 0, i32 1
	%106 = load i32, ptr %105, align 4
	call void @llvm.memcpy.p0.p0.i64(ptr align 4 %7, ptr align 4 %3, i64 16, i1 false)
	call void @make_pair2(ptr dead_on_unwind noalias writable sret(%struct.Pair3) align 8 %8, i32 noundef 17, i32 noundef 25, i64 noundef %104, i32 noundef %106, ptr noundef byval(%struct.Pair) align 4 %7)
	call void @llvm.memcpy.p0.p0.i64(ptr align 8 %6, ptr align 8 %8, i64 32, i1 false)
	br label %label_149

label_149:
	%107 = getelementptr inbounds nuw %struct.Pair, ptr %3, i32 0, i32 0
	%108 = load i32, ptr %107, align 4
	%109 = icmp eq i32 %108, 17
	%110 = xor i1 %109, 1
	%111 = icmp ne i1 %110, 0
	br i1 %111, label %label_151, label %label_150

label_151:
	%112 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.12, i32 noundef 264)
	%113 = load i32, ptr @failures, align 4
	%114 = add nsw i32 %113, 1
	store i32 %114, ptr @failures, align 4
	br label %label_150

label_150:
	br label %label_148

label_148:
	br label %label_161

label_161:
	%115 = getelementptr inbounds nuw %struct.Pair, ptr %3, i32 0, i32 2
	%116 = load i32, ptr %115, align 4
	%117 = icmp eq i32 %116, 25
	%118 = xor i1 %117, 1
	%119 = icmp ne i1 %118, 0
	br i1 %119, label %label_163, label %label_162

label_163:
	%120 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.13, i32 noundef 265)
	%121 = load i32, ptr @failures, align 4
	%122 = add nsw i32 %121, 1
	store i32 %122, ptr @failures, align 4
	br label %label_162

label_162:
	br label %label_160

label_160:
	br label %label_173

label_173:
	%123 = getelementptr inbounds { i64, i64 }, ptr %3, i32 0, i32 0
	%124 = load i64, ptr %123, align 4
	%125 = getelementptr inbounds { i64, i64 }, ptr %3, i32 0, i32 1
	%126 = load i64, ptr %125, align 4
	%127 = call i32 @pair_sum(i64 noundef %124, i64 noundef %126)
	%128 = icmp eq i32 %127, 42
	%129 = xor i1 %128, 1
	%130 = icmp ne i1 %129, 0
	br i1 %130, label %label_175, label %label_174

label_175:
	%131 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.14, i32 noundef 266)
	%132 = load i32, ptr @failures, align 4
	%133 = add nsw i32 %132, 1
	store i32 %133, ptr @failures, align 4
	br label %label_174

label_174:
	br label %label_172

label_172:
	br label %label_188

label_188:
	%134 = call i64 @mixed(i8 noundef 1, i16 noundef 2, i32 noundef 3, i64 noundef 4)
	%135 = icmp eq i64 %134, 10
	%136 = xor i1 %135, 1
	%137 = icmp ne i1 %136, 0
	br i1 %137, label %label_190, label %label_189

label_190:
	%138 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.15, i32 noundef 268)
	%139 = load i32, ptr @failures, align 4
	%140 = add nsw i32 %139, 1
	store i32 %140, ptr @failures, align 4
	br label %label_189

label_189:
	br label %label_187

label_187:
	br label %label_199

label_199:
	%141 = call i32 @triple(i32 noundef 7)
	%142 = call i32 @inc(i32 noundef %141)
	%143 = icmp eq i32 %142, 22
	%144 = xor i1 %143, 1
	%145 = icmp ne i1 %144, 0
	br i1 %145, label %label_201, label %label_200

label_201:
	%146 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.16, i32 noundef 270)
	%147 = load i32, ptr @failures, align 4
	%148 = add nsw i32 %147, 1
	store i32 %148, ptr @failures, align 4
	br label %label_200

label_200:
	br label %label_198

label_198:
	br label %label_211

label_211:
	%149 = call i32 @bump()
	%150 = icmp eq i32 %149, 1
	%151 = xor i1 %150, 1
	%152 = icmp ne i1 %151, 0
	br i1 %152, label %label_213, label %label_212

label_213:
	%153 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.17, i32 noundef 272)
	%154 = load i32, ptr @failures, align 4
	%155 = add nsw i32 %154, 1
	store i32 %155, ptr @failures, align 4
	br label %label_212

label_212:
	br label %label_210

label_210:
	br label %label_222

label_222:
	%156 = call i32 @bump()
	%157 = icmp eq i32 %156, 2
	%158 = xor i1 %157, 1
	%159 = icmp ne i1 %158, 0
	br i1 %159, label %label_224, label %label_223

label_224:
	%160 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.18, i32 noundef 273)
	%161 = load i32, ptr @failures, align 4
	%162 = add nsw i32 %161, 1
	store i32 %162, ptr @failures, align 4
	br label %label_223

label_223:
	br label %label_221

label_221:
	br label %label_233

label_233:
	%163 = call i32 @bump()
	%164 = icmp eq i32 %163, 3
	%165 = xor i1 %164, 1
	%166 = icmp ne i1 %165, 0
	br i1 %166, label %label_235, label %label_234

label_235:
	%167 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.19, i32 noundef 274)
	%168 = load i32, ptr @failures, align 4
	%169 = add nsw i32 %168, 1
	store i32 %169, ptr @failures, align 4
	br label %label_234

label_234:
	br label %label_232

label_232:
	br label %label_244

label_244:
	%170 = call i32 @f1(i32 noundef 10)
	%171 = call i32 @f1(i32 noundef 20)
	%172 = call i32 @f2(i32 noundef %170, i32 noundef %171)
	%173 = icmp eq i32 %172, 32
	%174 = xor i1 %173, 1
	%175 = icmp ne i1 %174, 0
	br i1 %175, label %label_246, label %label_245

label_246:
	%176 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr @.str.20, i32 noundef 276)
	%177 = load i32, ptr @failures, align 4
	%178 = add nsw i32 %177, 1
	store i32 %178, ptr @failures, align 4
	br label %label_245

label_245:
	br label %label_243

label_243:
	%179 = load i32, ptr @failures, align 4
	%180 = icmp eq i32 %179, 0
	%181 = icmp ne i1 %180, 0
	br i1 %181, label %label_257, label %label_256

label_257:
	%182 = call i32 (ptr, ...) @printf(ptr @.str.21)
	store i32 0, ptr %1, align 4
	br label %label_1

label_256:
	%183 = load i32, ptr @failures, align 4
	%184 = call i32 (ptr, ...) @printf(ptr @.str.22, i32 noundef %183)
	store i32 1, ptr %1, align 4
	br label %label_1

label_1:
	%185 = load i32, ptr %1, align 4
	ret i32 %185
}

declare i32 @printf(ptr noundef %0, ...) #1
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #2
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #3

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}
attributes #2 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #3 = { nocallback nofree nounwind willreturn memory(argmem: write) }