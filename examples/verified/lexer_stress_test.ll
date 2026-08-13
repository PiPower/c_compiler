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
	%39 = getelementptr inbounds nuw %union.Data, ptr %19, i32 0, i32 1
	store float 0x40091EB860000000, ptr %39, align 4
	call void @llvm.memset.p0.i64(ptr align 4 %20, i8 0, i64 20, i1 false)
	%40 = getelementptr inbounds [5 x i32], ptr %20, i64 0, i64 0
	store i32 1, ptr %40, align 4
	%41 = getelementptr inbounds [5 x i32], ptr %20, i64 0, i64 3
	store i32 4, ptr %41, align 4
	call void @llvm.memset.p0.i64(ptr align 4 %21, i8 0, i64 16, i1 false)
	%42 = getelementptr inbounds [2 x [2 x i32]], ptr %21, i64 0, i64 0, i64 0
	store i32 1, ptr %42, align 4
	%43 = getelementptr inbounds [2 x [2 x i32]], ptr %21, i64 0, i64 0, i64 1
	store i32 2, ptr %43, align 4
	%44 = getelementptr inbounds [2 x [2 x i32]], ptr %21, i64 0, i64 1, i64 0
	store i32 3, ptr %44, align 4
	%45 = getelementptr inbounds [2 x [2 x i32]], ptr %21, i64 0, i64 1, i64 1
	store i32 4, ptr %45, align 4
	store i32 5, ptr %22, align 4
	store i32 2, ptr %23, align 4
	%46 = load i32, ptr %22, align 4
	%47 = load i32, ptr %23, align 4
	%48 = add nsw i32 %46, %47
	%49 = load i32, ptr %22, align 4
	%50 = load i32, ptr %23, align 4
	%51 = mul nsw i32 %49, %50
	%52 = load i32, ptr %23, align 4
	%53 = sdiv i32 %51, %52
	%54 = load i32, ptr %22, align 4
	%55 = srem i32 %53, %54
	%56 = sub nsw i32 %48, %55
	store i32 %56, ptr %24, align 4
	%57 = load i32, ptr %22, align 4
	%58 = load i32, ptr %23, align 4
	%59 = icmp sgt i32 %57, %58
	%60 = icmp ne i1 %59, 0
	br i1 %60, label %label_57, label %label_51

label_57:
	%61 = load i32, ptr %23, align 4
	%62 = icmp slt i32 %61, 10
	%63 = icmp ne i1 %62, 0
	br i1 %63, label %label_52, label %label_51

label_52:
	store i8 1, ptr %27, align 1
	br label %label_61

label_51:
	store i8 0, ptr %27, align 1
	br label %label_61

label_61:
	%64 = load i8, ptr %27, align 1
	%65 = icmp ne i8 %64, 0
	br i1 %65, label %label_49, label %label_64

label_64:
	%66 = load i32, ptr %22, align 4
	%67 = load i32, ptr %23, align 4
	%68 = icmp eq i32 %66, %67
	%69 = xor i1 %68, 1
	%70 = icmp ne i1 %69, 0
	br i1 %70, label %label_49, label %label_48

label_49:
	store i8 1, ptr %26, align 1
	br label %label_70

label_48:
	store i8 0, ptr %26, align 1
	br label %label_70

label_70:
	%71 = load i8, ptr %26, align 1
	%72 = zext i8 %71 to i32
	store i32 %72, ptr %25, align 4
	%73 = load i32, ptr %22, align 4
	%74 = load i32, ptr %23, align 4
	%75 = and i32 %73, %74
	%76 = load i32, ptr %22, align 4
	%77 = load i32, ptr %23, align 4
	%78 = xor i32 %76, %77
	%79 = shl i32 %78, 1
	%80 = ashr i32 %79, 1
	%81 = or i32 %75, %80
	store i32 %81, ptr %28, align 4
	%82 = load i32, ptr %22, align 4
	%83 = load i32, ptr %23, align 4
	%84 = icmp sgt i32 %82, %83
	%85 = icmp ne i1 %84, 0
	br i1 %85, label %label_84, label %label_85

label_84:
	%86 = load i32, ptr %22, align 4
	br label %label_86

label_85:
	%87 = load i32, ptr %23, align 4
	br label %label_86

label_86:
	%88 = phi i32 [ %86, %label_84 ], [ %87, %label_85 ]
	store i32 %88, ptr %29, align 4
	%89 = load i32, ptr %22, align 4
	%90 = icmp sgt i32 %89, 0
	%91 = icmp ne i1 %90, 0
	br i1 %91, label %label_95, label %label_96

label_95:
	store i32 0, ptr %30, align 4
	br label %label_103

label_103:
	%92 = load i32, ptr %30, align 4
	%93 = icmp slt i32 %92, 5
	%94 = icmp ne i1 %93, 0
	br i1 %94, label %label_100, label %label_101

label_100:
	%95 = load i32, ptr %30, align 4
	%96 = call i32 (ptr, ...) @printf(ptr @.str.5, i32 noundef %95)
	%97 = load i32, ptr %30, align 4
	%98 = add nsw i32 %97, 1
	store i32 %98, ptr %30, align 4
	br label %label_103

label_101:
	br label %label_94

label_96:
	%99 = load i32, ptr %22, align 4
	%100 = icmp eq i32 %99, 0
	%101 = icmp ne i1 %100, 0
	br i1 %101, label %label_111, label %label_112

label_111:
	%102 = call i32 (ptr, ...) @printf(ptr @.str.6)
	br label %label_94

label_112:
	%103 = call i32 (ptr, ...) @printf(ptr @.str.7)
	br label %label_94

label_94:
	%104 = load i32, ptr %22, align 4
	switch i32 %104, label %label_121 [
		 i32 1, label %label_119
		 i32 5, label %label_120
	]

label_119:
	br label %label_118

label_120:
	br label %label_123

label_121:
	br label %label_118

label_118:
	br label %label_123

label_123:
	br label %label_124

label_124:
	%105 = load i32, ptr %23, align 4
	%106 = sub nsw i32 %105, 1
	store i32 %106, ptr %23, align 4
	%107 = icmp ne i32 %105, 0
	br i1 %107, label %label_125, label %label_126

label_125:
	br label %label_124

label_126:
	br label %label_131

label_131:
	%108 = load i32, ptr %22, align 4
	%109 = sub nsw i32 %108, 1
	store i32 %109, ptr %22, align 4
	%110 = load i32, ptr %22, align 4
	%111 = icmp sgt i32 %110, 0
	%112 = icmp ne i1 %111, 0
	br i1 %112, label %label_131, label %label_130

label_130:
	store i64 8, ptr %31, align 8
	%113 = load i32, ptr %22, align 4
	%114 = sitofp i32 %113 to double
	store double %114, ptr %32, align 8
	store i32 10, ptr %33, align 4
	store i32 20, ptr %34, align 4
	store ptr %33, ptr %35, align 8
	store ptr %34, ptr %36, align 8
	%115 = load ptr, ptr %35, align 8
	%116 = load i32, ptr %115, align 4
	%117 = load ptr, ptr %36, align 8
	%118 = load i32, ptr %117, align 4
	%119 = add nsw i32 %116, %118
	%120 = load ptr, ptr %35, align 8
	store i32 %119, ptr %120, align 4
	%121 = call i32 (ptr, ...) @printf(ptr @.str.8)
	store i32 0, ptr %1, align 4
	br label %label_1

label_1:
	%122 = load i32, ptr %1, align 4
	ret i32 %122
}

declare i32 @printf(ptr noundef %0, ...) #1
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }