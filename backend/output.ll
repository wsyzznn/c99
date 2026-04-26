; --- LLVM IR Output ---
  %1 = load i32, i32* %b
  %2 = load i32, i32* %c
  %3 = mul i32 %2, 2
  %4 = add i32 %1, %3
  store i32 %4, i32* %a
  %5 = load i32, i32* %a
  %6 = icmp slt i32 %5, 100
  %7 = zext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %L1, label %L2
L1:
  %9 = load i32, i32* %a
  %10 = add i32 %9, 1
  store i32 %10, i32* %a
  br label %L3
L2:
  %11 = load i32, i32* %a
  %12 = sub i32 %11, 1
  store i32 %12, i32* %a
  br label %L3
L3:
  br label %L4
L4:
  %13 = load i32, i32* %b
  %14 = icmp sgt i32 %13, 0
  %15 = zext i1 %14 to i32
  %16 = icmp ne i32 %15, 0
  br i1 %16, label %L5, label %L6
L5:
  %17 = load i32, i32* %b
  %18 = sub i32 %17, 1
  store i32 %18, i32* %b
  br label %L4
L6:
