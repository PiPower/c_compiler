%struct._IO_FILE = type { i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i32, i64, i16, i8, [1 x i8], ptr, i64, ptr, ptr, ptr, ptr, i64, i32, [20 x i8] }
%struct.Point = type { i32, i32 }

@stderr = external global ptr
@stdout = external global ptr
@stdin = external global ptr

define dso_local i32 @main() #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 939, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 395995, ptr %4, align 4
	%5 = alloca i8, align 1
	store i8 92, ptr %5, align 1
	%6 = alloca i32, align 4
	store i32 5, ptr %6, align 4
	%7 = alloca i32, align 4
	store i32 10, ptr %7, align 4
	%8 = alloca float, align 4
	store float 3.140000e+00, ptr %8, align 4
	%9 = alloca float, align 4
	store float 0.000000e+00, ptr %9, align 4
	%10 = alloca double, align 8
	store double 1.000000e+00, ptr %10, align 8
	%11 = alloca i64, align 8
	store i64 1234567890, ptr %11, align 8
	%12 = alloca i8, align 1
	store i8 90, ptr %12, align 1
	%13 = alloca i8, align 1
	store i8 1, ptr %13, align 1
	%14 = alloca i32, align 4
	store i32 42, ptr %14, align 4
	%15 = alloca i32, align 4
	store i32 0, ptr %15, align 4
	%19 = icmp ne void %-10000, 
	br i1 %19, label %label_17, label %label_18

label_17:
	%20 = load i32, ptr %6, align 4
	%21 = add nsw i32 %20, 1
	store i32 %21, ptr %6, align 4
	br label %label_16

label_18:
	%22 = load i32, ptr %6, align 4
	%23 = mul nsw i32 %22, 2
	store i32 %23, ptr %6, align 4
	%24 = load i32, ptr %7, align 4
	%25 = sdiv i32 %24, 2
	store i32 %25, ptr %7, align 4
	%26 = load float, ptr %8, align 4
	%27 = load float, ptr %9, align 4
	%28 = fmul float %26, %27
	store float %28, ptr %8, align 4
	%29 = load float, ptr %9, align 4
	%30 = load float, ptr %8, align 4
	%31 = fdiv float %29, %30
	store float %31, ptr %9, align 4
	br label %label_16

label_16:
	%35 = load i32, ptr %6, align 4
	switch i32 %35, label %label_34 [
		 i32 1, label %label_33
	]

label_33:
	br label %label_32

label_34:
	br label %label_32

label_32:
	br label %label_36

label_36:
	%39 = load i32, ptr %6, align 4
	%40 = icmp slt i32 %39, 100
	%41 = icmp ne i1 %40, 0
	br i1 %41, label %label_37, label %label_38

label_37:
	%42 = load i32, ptr %6, align 4
	%43 = shl i32 %42, 1
	store i32 %43, ptr %6, align 4
	%44 = load i32, ptr %7, align 4
	%45 = ashr i32 %44, 1
	store i32 %45, ptr %7, align 4
	br label %label_36

label_38:
	%46 = alloca %struct.Point, align 4
	%47 = alloca ptr, align 8
	store ptr %46, ptr %47, align 8
	store i32 0, ptr %2, align 4
	br label %label_1

label_1:
	%48 = load i32, ptr %2, align 4
	ret i32 %48
}


attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}