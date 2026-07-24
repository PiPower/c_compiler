%struct._IO_FILE = type { i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i32, i64, i16, i8, [1 x i8], ptr, i64, ptr, ptr, ptr, ptr, i64, i32, [20 x i8] }
%struct.Point = type { i32, i32 }
%struct.PointNested = type { i32, i32, %struct.Point }
%struct.Line = type { %struct.Point, %struct.Point }

@.str.1 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [13 x i8] c"%d %d %d %d\0A\00", align 1
@stderr = external global ptr
@stdout = external global ptr
@stdin = external global ptr

define dso_local i32 @main() #0 {
	%1 = alloca i32, align 4
	%2 = alloca %struct.Point, align 4
	%3 = alloca %struct.PointNested, align 4
	%4 = alloca ptr, align 8
	%5 = alloca %struct.Line, align 4
	%6 = alloca ptr, align 8
	%7 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 0
	store i32 1, ptr %7, align 4
	%8 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 1
	store i32 2, ptr %8, align 4
	%9 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 0
	%10 = getelementptr inbounds nuw %struct.PointNested, ptr %3, i32 0, i32 2
	%11 = getelementptr inbounds nuw %struct.Point, ptr %10, i32 0, i32 0
	%12 = load i32, ptr %9, align 4
	store i32 %12, ptr %11, align 4
	%13 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 0
	%14 = load i32, ptr %13, align 4
	%15 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 1
	%16 = load i32, ptr %15, align 4
	%17 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 noundef %14, i32 noundef %16)
	store ptr %2, ptr %4, align 8
	%18 = load ptr, ptr %4, align 8
	%19 = getelementptr inbounds nuw %struct.Point, ptr %18, i32 0, i32 0
	store i32 3, ptr %19, align 4
	%20 = load ptr, ptr %4, align 8
	%21 = getelementptr inbounds nuw %struct.Point, ptr %20, i32 0, i32 1
	store i32 4, ptr %21, align 4
	%22 = load ptr, ptr %4, align 8
	%23 = getelementptr inbounds nuw %struct.Point, ptr %22, i32 0, i32 0
	%24 = load i32, ptr %23, align 4
	%25 = load ptr, ptr %4, align 8
	%26 = getelementptr inbounds nuw %struct.Point, ptr %25, i32 0, i32 1
	%27 = load i32, ptr %26, align 4
	%28 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 noundef %24, i32 noundef %27)
	%29 = load ptr, ptr %4, align 8
	%30 = getelementptr inbounds nuw %struct.Point, ptr %29, i32 0, i32 0
	store i32 5, ptr %30, align 4
	%31 = load ptr, ptr %4, align 8
	%32 = getelementptr inbounds nuw %struct.Point, ptr %31, i32 0, i32 1
	store i32 6, ptr %32, align 4
	%33 = load ptr, ptr %4, align 8
	%34 = getelementptr inbounds nuw %struct.Point, ptr %33, i32 0, i32 0
	%35 = load i32, ptr %34, align 4
	%36 = load ptr, ptr %4, align 8
	%37 = getelementptr inbounds nuw %struct.Point, ptr %36, i32 0, i32 1
	%38 = load i32, ptr %37, align 4
	%39 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 noundef %35, i32 noundef %38)
	%40 = getelementptr inbounds nuw %struct.Line, ptr %5, i32 0, i32 0
	%41 = getelementptr inbounds nuw %struct.Point, ptr %40, i32 0, i32 0
	store i32 10, ptr %41, align 4
	%42 = getelementptr inbounds nuw %struct.Line, ptr %5, i32 0, i32 0
	%43 = getelementptr inbounds nuw %struct.Point, ptr %42, i32 0, i32 1
	store i32 20, ptr %43, align 4
	%44 = getelementptr inbounds nuw %struct.Line, ptr %5, i32 0, i32 1
	%45 = getelementptr inbounds nuw %struct.Point, ptr %44, i32 0, i32 0
	store i32 30, ptr %45, align 4
	%46 = getelementptr inbounds nuw %struct.Line, ptr %5, i32 0, i32 1
	%47 = getelementptr inbounds nuw %struct.Point, ptr %46, i32 0, i32 1
	store i32 40, ptr %47, align 4
	%48 = getelementptr inbounds nuw %struct.Line, ptr %5, i32 0, i32 0
	%49 = getelementptr inbounds nuw %struct.Point, ptr %48, i32 0, i32 0
	%50 = load i32, ptr %49, align 4
	%51 = getelementptr inbounds nuw %struct.Line, ptr %5, i32 0, i32 0
	%52 = getelementptr inbounds nuw %struct.Point, ptr %51, i32 0, i32 1
	%53 = load i32, ptr %52, align 4
	%54 = getelementptr inbounds nuw %struct.Line, ptr %5, i32 0, i32 1
	%55 = getelementptr inbounds nuw %struct.Point, ptr %54, i32 0, i32 0
	%56 = load i32, ptr %55, align 4
	%57 = getelementptr inbounds nuw %struct.Line, ptr %5, i32 0, i32 1
	%58 = getelementptr inbounds nuw %struct.Point, ptr %57, i32 0, i32 1
	%59 = load i32, ptr %58, align 4
	%60 = call i32 (ptr, ...) @printf(ptr @.str.2, i32 noundef %50, i32 noundef %53, i32 noundef %56, i32 noundef %59)
	store ptr %5, ptr %6, align 8
	br label %label_1

label_1:
	%61 = load i32, ptr %1, align 4
	ret i32 %61
}

declare i32 @printf(ptr noundef %0, ...) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}