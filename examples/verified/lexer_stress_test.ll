%struct._IO_FILE = type { i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i32, i64, i16, i8, [1 x i8], ptr, i64, ptr, ptr, ptr, ptr, i64, i32, [20 x i8] }
%struct.anon.2 = type { i32, i32 }
%struct.anon.3 = type { i64, i64 }
%struct.anon.4 = type { i64, i64 }
%struct.Point = type { i32, i32 }
%union.Data = type { i32, [16 x i8] }

@.str.1 = private unnamed_addr constant [12 x i8] c"Result: %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [14 x i8] c"Hello, World!\00", align 1
@.str.3 = private unnamed_addr constant [23 x i8] c"Escaped chars: \09 \0A \22 \5C\00", align 1
@.str.4 = private unnamed_addr constant [14 x i8] c"Concatenated \00", align 1
@.str.5 = private unnamed_addr constant [8 x i8] c"i = %d\0A\00", align 1
@.str.6 = private unnamed_addr constant [6 x i8] c"Zero\0A\00", align 1
@.str.7 = private unnamed_addr constant [10 x i8] c"Negative\0A\00", align 1
@.str.8 = private unnamed_addr constant [7 x i8] c"Done.\0A\00", align 1
@global_var = dso_local global i32 42, align 4
@cv_var = dso_local global i32 100, align 4
@stderr = external global ptr
@stdout = external global ptr
@stdin = external global ptr

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

define dso_local void @function_pointer_test() #0 {
	%1 = alloca ptr, align 8
	%2 = alloca i32, align 4
	store ptr @add, ptr %1, align 8
	%3 = load ptr, ptr %1, align 8
	%4 = call i32 %3(i32 noundef 3, i32 noundef 4)
	store i32 %4, ptr %2, align 4
	%5 = load i32, ptr %2, align 4
	%6 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 noundef %5)
	br label %label_1

label_1:
	ret void
}

define dso_local i32 @main() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	%6 = alloca i64, align 8
	%7 = alloca i64, align 8
	%8 = alloca float, align 4
	%9 = alloca double, align 8
	%10 = alloca double, align 8
	%11 = alloca double, align 8
	%12 = alloca i8, align 1
	%13 = alloca i8, align 1
	%14 = alloca i8, align 1
	%15 = alloca ptr, align 8
	%16 = alloca ptr, align 8
	%17 = alloca ptr, align 8
	%18 = alloca %struct.Point, align 4
	%19 = alloca %union.Data, align 4
	%20 = alloca [5 x i32], align 4
	%21 = alloca [2 x [2 x i32]], align 4
	%22 = alloca i32, align 4
	%23 = alloca i32, align 4
	%24 = alloca i32, align 4
	%25 = alloca i32, align 4
	%26 = alloca i8, align 1
	%27 = alloca i8, align 1
	%28 = alloca i32, align 4
	%29 = alloca i32, align 4
	%30 = alloca i32, align 4
	%31 = alloca i64, align 8
	%32 = alloca double, align 8
	%33 = alloca i32, align 4
	%34 = alloca i32, align 4
	%35 = alloca ptr, align 8
	%36 = alloca ptr, align 8
	store i32 123, ptr %2, align 4
	store i32 123, ptr %3, align 4
	store i32 0, ptr %4, align 4
	store i32 123, ptr %5, align 4
	store i64 1234567890, ptr %6, align 8
	store i64 123456789012345, ptr %7, align 8
	store float 0x40091EB860000000, ptr %8, align 4
	store double 0x4005BF0995AAF790, ptr %9, align 8
	store double 0x3FF3AE147AE147AE, ptr %10, align 8
	store double 0x0000000000000000, ptr %11, align 8
	store i8 97, ptr %12, align 1
	store i8 92, ptr %13, align 1
	store i8 92, ptr %14, align 1
	store ptr @.str.2, ptr %15, align 8
	store ptr @.str.3, ptr %16, align 8
	store ptr @.str.4, ptr %17, align 8
	call void @llvm.memset.p0.i64(ptr align 4 %18, i8 0, i64 8, i1 false)
	%37 = getelementptr inbounds nuw %struct.Point, ptr %18, i32 0, i32 0
	store i32 10, ptr %37, align 4
	%38 = getelementptr inbounds nuw %struct.Point, ptr %18, i32 0, i32 1
	store i32 20, ptr %38, align 4
	call void @llvm.memset.p0.i64(ptr align 4 %20, i8 0, i64 20, i1 false)
	%39 = getelementptr inbounds [5 x i32], ptr %20, i64 0, i64 0
	store i32 1, ptr %39, align 4
	%40 = getelementptr inbounds [5 x i32], ptr %20, i64 0, i64 3
	store i32 4, ptr %40, align 4
	call void @llvm.memset.p0.i64(ptr align 4 %21, i8 0, i64 16, i1 false)
	%41 = getelementptr inbounds [2 x [2 x i32]], ptr %21, i64 0, i64 0, i64 0
	store i32 1, ptr %41, align 4
	%42 = getelementptr inbounds [2 x [2 x i32]], ptr %21, i64 0, i64 0, i64 1
	store i32 2, ptr %42, align 4
	%43 = getelementptr inbounds [2 x [2 x i32]], ptr %21, i64 0, i64 1, i64 0
	store i32 3, ptr %43, align 4
	%44 = getelementptr inbounds [2 x [2 x i32]], ptr %21, i64 0, i64 1, i64 1
	store i32 4, ptr %44, align 4
	store i32 5, ptr %22, align 4
	store i32 2, ptr %23, align 4
	%45 = load i32, ptr %22, align 4
	%46 = load i32, ptr %23, align 4
	%47 = add nsw i32 %45, %46
	%48 = load i32, ptr %22, align 4
	%49 = load i32, ptr %23, align 4
	%50 = mul nsw i32 %48, %49
	%51 = load i32, ptr %23, align 4
	%52 = sdiv i32 %50, %51
	%53 = load i32, ptr %22, align 4
	%54 = srem i32 %52, %53
	%55 = sub nsw i32 %47, %54
	store i32 %55, ptr %24, align 4
	%56 = load i32, ptr %22, align 4
	%57 = load i32, ptr %23, align 4
	%58 = icmp sgt i32 %56, %57
	%59 = icmp ne i1 %58, 0
	br i1 %59, label %label_56, label %label_50

label_56:
	%60 = load i32, ptr %23, align 4
	%61 = icmp slt i32 %60, 10
	%62 = icmp ne i1 %61, 0
	br i1 %62, label %label_51, label %label_50

label_51:
	store i8 1, ptr %27, align 1
	br label %label_60

label_50:
	store i8 0, ptr %27, align 1
	br label %label_60

label_60:
	%63 = load i8, ptr %27, align 1
	%64 = icmp ne i8 %63, 0
	br i1 %64, label %label_48, label %label_63

label_63:
	%65 = load i32, ptr %22, align 4
	%66 = load i32, ptr %23, align 4
	%67 = icmp eq i32 %65, %66
	%68 = xor i1 %67, 1
	%69 = icmp ne i1 %68, 0
	br i1 %69, label %label_48, label %label_47

label_48:
	store i8 1, ptr %26, align 1
	br label %label_69

label_47:
	store i8 0, ptr %26, align 1
	br label %label_69

label_69:
	%70 = load i8, ptr %26, align 1
	%71 = zext i8 %70 to i32
	store i32 %71, ptr %25, align 4
	%72 = load i32, ptr %22, align 4
	%73 = load i32, ptr %23, align 4
	%74 = and i32 %72, %73
	%75 = load i32, ptr %22, align 4
	%76 = load i32, ptr %23, align 4
	%77 = xor i32 %75, %76
	%78 = shl i32 %77, 1
	%79 = ashr i32 %78, 1
	%80 = or i32 %74, %79
	store i32 %80, ptr %28, align 4
	%81 = load i32, ptr %22, align 4
	%82 = load i32, ptr %23, align 4
	%83 = icmp sgt i32 %81, %82
	%84 = icmp ne i1 %83, 0
	br i1 %84, label %label_83, label %label_84

label_83:
	%85 = load i32, ptr %22, align 4
	br label %label_85

label_84:
	%86 = load i32, ptr %23, align 4
	br label %label_85

label_85:
	%87 = phi i32 [ %85, %label_83 ], [ %86, %label_84 ]
	store i32 %87, ptr %29, align 4
	%88 = load i32, ptr %22, align 4
	%89 = icmp sgt i32 %88, 0
	%90 = icmp ne i1 %89, 0
	br i1 %90, label %label_94, label %label_95

label_94:
	store i32 0, ptr %30, align 4
	br label %label_102

label_102:
	%91 = load i32, ptr %30, align 4
	%92 = icmp slt i32 %91, 5
	%93 = icmp ne i1 %92, 0
	br i1 %93, label %label_99, label %label_100

label_99:
	%94 = load i32, ptr %30, align 4
	%95 = call i32 (ptr, ...) @printf(ptr @.str.5, i32 noundef %94)
	%96 = load i32, ptr %30, align 4
	%97 = add nsw i32 %96, 1
	store i32 %97, ptr %30, align 4
	br label %label_102

label_100:
	br label %label_93

label_95:
	%98 = load i32, ptr %22, align 4
	%99 = icmp eq i32 %98, 0
	%100 = icmp ne i1 %99, 0
	br i1 %100, label %label_110, label %label_111

label_110:
	%101 = call i32 (ptr, ...) @printf(ptr @.str.6)
	br label %label_93

label_111:
	%102 = call i32 (ptr, ...) @printf(ptr @.str.7)
	br label %label_93

label_93:
	%103 = load i32, ptr %22, align 4
	switch i32 %103, label %label_120 [
		 i32 1, label %label_118
		 i32 5, label %label_119
	]

label_118:
	br label %label_117

label_119:
	br label %label_122

label_120:
	br label %label_117

label_117:
	br label %label_122

label_122:
	br label %label_123

label_123:
	%104 = load i32, ptr %23, align 4
	%105 = sub nsw i32 %104, 1
	store i32 %105, ptr %23, align 4
	%106 = icmp ne i32 %104, 0
	br i1 %106, label %label_124, label %label_125

label_124:
	br label %label_123

label_125:
	br label %label_130

label_130:
	%107 = load i32, ptr %22, align 4
	%108 = sub nsw i32 %107, 1
	store i32 %108, ptr %22, align 4
	%109 = load i32, ptr %22, align 4
	%110 = icmp sgt i32 %109, 0
	%111 = icmp ne i1 %110, 0
	br i1 %111, label %label_130, label %label_129

label_129:
	store i64 8, ptr %31, align 8
	%112 = load i32, ptr %22, align 4
	%113 = sitofp i32 %112 to double
	store double %113, ptr %32, align 8
	store i32 10, ptr %33, align 4
	store i32 20, ptr %34, align 4
	store ptr %33, ptr %35, align 8
	store ptr %34, ptr %36, align 8
	%114 = load ptr, ptr %35, align 8
	%115 = load i32, ptr %114, align 4
	%116 = load ptr, ptr %36, align 8
	%117 = load i32, ptr %116, align 4
	%118 = add nsw i32 %115, %117
	%119 = load ptr, ptr %35, align 8
	store i32 %118, ptr %119, align 4
	%120 = call i32 (ptr, ...) @printf(ptr @.str.8)
	store i32 0, ptr %1, align 4
	br label %label_1

label_1:
	%121 = load i32, ptr %1, align 4
	ret i32 %121
}

declare i32 @printf(ptr noundef %0, ...) #1
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }