

define dso_local i32 @sum_point(i64 noundef %0) #0 {
	%2 = alloca %struct.Point, align 4
	store i64 %0, ptr %2, align 8
	%4 = alloca i32, align 4
	%5 = getelementptr inbounds %struct.Point, ptr %2, i64 0, i64 0
	%6 = getelementptr inbounds %struct.Point, ptr %2, i64 0, i64 1
	%7 = add nsw i32 %5, %6
	store i32 %7, ptr %4, align 4
	br label %label_3

label_3:
	%8 = load i32, ptr %4, align 4
	ret i32 %8
}


attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}