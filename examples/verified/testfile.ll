%struct._IO_FILE = type { i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i32, i64, i16, i8, [1 x i8], ptr, i64, ptr, ptr, ptr, ptr, i64, i32, [20 x i8] }
%struct.Point = type { i32, i32 }

@stderr = external global ptr
@stdout = external global ptr
@stdin = external global ptr

define dso_local i32 @main() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca i8, align 1
	%5 = alloca i32, align 4
	%6 = alloca i32, align 4
	%7 = alloca float, align 4
	%8 = alloca float, align 4
	%9 = alloca double, align 8
	%10 = alloca i64, align 8
	%11 = alloca i8, align 1
	%12 = alloca i8, align 1
	%13 = alloca i32, align 4
	%14 = alloca i32, align 4
	%15 = alloca i8, align 1
	%16 = alloca i8, align 1
	%17 = alloca %struct.Point, align 4
	%18 = alloca ptr, align 8
	store i32 0, ptr %1, align 4
	store i32 939, ptr %2, align 4
	store i32 395995, ptr %3, align 4
	store i8 92, ptr %4, align 1
	store i32 5, ptr %5, align 4
	store i32 10, ptr %6, align 4
	store float 0x40091EB860000000, ptr %7, align 4
	store float 0x0000000000000000, ptr %8, align 4
	store double 0x3FF0000000000000, ptr %9, align 8
	store i64 1234567890, ptr %10, align 8
	store i8 90, ptr %11, align 1
	store i8 1, ptr %12, align 1
	store i32 42, ptr %13, align 4
	store i32 0, ptr %14, align 4
	%19 = load i32, ptr %5, align 4
	%20 = load i32, ptr %6, align 4
	%21 = mul nsw i32 %19, %20
	%22 = icmp slt i32 %21, 22
	%23 = icmp ne i1 %22, 0
	br i1 %23, label %label_21, label %label_27

label_27:
	%24 = load i32, ptr %5, align 4
	%25 = load i32, ptr %6, align 4
	%26 = icmp slt i32 %24, %25
	%27 = icmp ne i1 %26, 0
	br i1 %27, label %label_35, label %label_29

label_35:
	%28 = load i32, ptr %6, align 4
	%29 = icmp sgt i32 %28, 0
	%30 = icmp ne i1 %29, 0
	br i1 %30, label %label_39, label %label_29

label_39:
	%31 = load i32, ptr %6, align 4
	%32 = icmp slt i32 %31, 11
	%33 = icmp ne i1 %32, 0
	br i1 %33, label %label_30, label %label_29

label_30:
	store i8 1, ptr %16, align 1
	br label %label_43

label_29:
	store i8 0, ptr %16, align 1
	br label %label_43

label_43:
	%34 = load i8, ptr %16, align 1
	%35 = icmp ne i8 %34, 0
	br i1 %35, label %label_21, label %label_46

label_46:
	%36 = load i8, ptr %12, align 1
	%37 = xor i8 %36, 1
	%38 = icmp ne i8 %37, 0
	br i1 %38, label %label_21, label %label_20

label_21:
	store i8 1, ptr %15, align 1
	br label %label_50

label_20:
	store i8 0, ptr %15, align 1
	br label %label_50

label_50:
	%39 = load i8, ptr %15, align 1
	%40 = icmp ne i8 %39, 0
	br i1 %40, label %label_17, label %label_18

label_17:
	%41 = load i32, ptr %5, align 4
	%42 = add nsw i32 %41, 1
	store i32 %42, ptr %5, align 4
	%43 = load i32, ptr %6, align 4
	%44 = sub nsw i32 %43, 1
	store i32 %44, ptr %6, align 4
	%45 = load i8, ptr %11, align 1
	%46 = sext i8 %45 to i32
	%47 = add nsw i32 %46, 1
	%48 = trunc i32 %47 to i8
	store i8 %48, ptr %11, align 1
	%49 = load double, ptr %9, align 8
	%50 = fsub double %49, 1.000000
	store double %50, ptr %9, align 8
	br label %label_16

label_18:
	%51 = load i32, ptr %5, align 4
	%52 = mul nsw i32 %51, 2
	store i32 %52, ptr %5, align 4
	%53 = load i32, ptr %6, align 4
	%54 = sdiv i32 %53, 2
	store i32 %54, ptr %6, align 4
	%55 = load float, ptr %7, align 4
	%56 = load float, ptr %8, align 4
	%57 = fmul float %55, %56
	store float %57, ptr %7, align 4
	%58 = load float, ptr %8, align 4
	%59 = load float, ptr %7, align 4
	%60 = fdiv float %58, %59
	store float %60, ptr %8, align 4
	br label %label_16

label_16:
	%61 = load i32, ptr %5, align 4
	switch i32 %61, label %label_75 [
		 i32 1, label %label_74
	]

label_74:
	br label %label_73

label_75:
	br label %label_73

label_73:
	br label %label_77

label_77:
	%62 = load i32, ptr %5, align 4
	%63 = icmp slt i32 %62, 100
	%64 = icmp ne i1 %63, 0
	br i1 %64, label %label_78, label %label_79

label_78:
	%65 = load i32, ptr %5, align 4
	%66 = shl i32 %65, 1
	store i32 %66, ptr %5, align 4
	%67 = load i32, ptr %6, align 4
	%68 = ashr i32 %67, 1
	store i32 %68, ptr %6, align 4
	br label %label_77

label_79:
	call void @llvm.memset.p0.i64(ptr align 4 %17, i8 0, i64 8, i1 false)
	%69 = getelementptr inbounds nuw %struct.Point, ptr %17, i32 0, i32 0
	%70 = load i32, ptr %5, align 4
	store i32 %70, ptr %69, align 4
	%71 = getelementptr inbounds nuw %struct.Point, ptr %17, i32 0, i32 1
	%72 = load i32, ptr %6, align 4
	store i32 %72, ptr %71, align 4
	store ptr %17, ptr %18, align 8
	store i32 0, ptr %1, align 4
	br label %label_1

label_1:
	%73 = load i32, ptr %1, align 4
	ret i32 %73
}

declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }