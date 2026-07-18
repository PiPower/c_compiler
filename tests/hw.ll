%struct._IO_FILE = type { i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i32, i64, i16, i8, [1 x i8], ptr, i64, ptr, ptr, ptr, ptr, i64, i32, [20 x i8] }
%struct.simple_struct = type { i32, [4 x [14 x ptr]], ptr, [6 x %struct.simple_struct_2], %struct.simple_struct_2, ptr, i32, i64 }
%struct.simple_struct_2 = type { i32, i32, i64 }
%struct.Outer = type { i64, i64, ptr, %struct.anon.2, %union.anon.1, [4 x [14 x ptr]], ptr, %struct.Nested, [3 x [2 x ptr]], ptr, %union.anon.3, ptr, [6 x %struct.anon.8], [0 x i8] }
%struct.anon.2 = type { i32, i32, i64, i8 }
%union.anon.1 = type { %struct.anon.4 }
%struct.Nested = type { i64, %union.anon.2, [2 x %struct.InnerMost] }
%union.anon.3 = type { i32 }
%struct.anon.8 = type { %union.anon.4 }
%struct.anon.4 = type { double, double }
%union.anon.2 = type { %struct.anon.5, [28 x i8] }
%struct.InnerMost = type { ptr, ptr }
%union.anon.4 = type { %struct.anon.9 }
%struct.anon.5 = type { i16, i16, [0 x i32] }
%struct.anon.9 = type { i32, i32, i32 }

@yolo = dso_local global %struct.Outer zeroinitializer, align 8
@prkw = dso_local global %struct.simple_struct zeroinitializer, align 8
@stderr = external global ptr
@stdout = external global ptr
@stdin = external global ptr

define dso_local i32 @main() #0 {
	%2 = alloca i32, align 4
	br label %label_1

label_1:
	%3 = load i32, ptr %2, align 4
	ret i32 %3
}


attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}