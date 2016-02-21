# yascm

[![Build Status](https://travis-ci.org/hmgle/yascm.png?branch=master)](https://travis-ci.org/hmgle/yascm)

Yet Another Scheme Interpreter.

## Building

The following packages need to install before build:

- flex
- bison

Debian/Ubuntu Installation:

```
sudo apt-get install flex bison
```

Then run the following:

```
git clone https://github.com/hmgle/yascm.git
cd yascm
make
```

If having trouble installing flex/bison, or you don't want to install flex/bison, you can clone the no-flex-bison branch:

```
git clone -b no-flex-bison https://github.com/hmgle/yascm.git
cd yascm
make
```

## Examples

- Recursion

```
$ ./yascm
welcome
> (define (sum x)
    (if (= 0 x) 0 (+ x (sum (- x 1)))))
; ok
> (sum 10000)
50005000
>
```

- Closure

```
$ ./yascm
welcome
> (define (add a)
    (lambda (b) (+ a b)))
; ok
> (define add3 (add 3))
; ok
> (add3 4)
7
> (define my-counter
    ((lambda (count)
       (lambda ()
         (set! count (+ count 1))
         count))
     0)
  )
; ok
> (my-counter)
1
> (my-counter)
2
> (my-counter)
3
>
```

- [Man or boy test](https://en.wikipedia.org/?title=Man_or_boy_test)

```
$ ./yascm
welcome
> (define (A k x1 x2 x3 x4 x5)
    (define (B)
      (set! k (- k 1))
      (A k B x1 x2 x3 x4))
    (if (> 1 k)
        (+ (x4) (x5))
        (B)))
; ok
> (A 10 (lambda () 1) (lambda () -1) (lambda () -1) (lambda () 1) (lambda () 0))
-67
> 
```

- [Y combinator](http://rosettacode.org/wiki/Y_combinator#Scheme)

```
$ ./yascm
; loading stdlib.scm
; done loading stdlib.scm
welcome
> (define Y
    (lambda (h)
      ((lambda (x) (x x))
       (lambda (g)
         (h (lambda args (apply (g g) args)))))))
; ok
> (define fib
    (Y
      (lambda (f)
        (lambda (x)
          (if (> 2 x)
              x
              (+ (f (- x 1)) (f (- x 2))))))))
; ok
> (fib 10)
55
```

- [The Metacircular Evaluator](https://mitpress.mit.edu/sicp/full-text/book/book-Z-H-26.html#%_sec_4.1)

```
$ ./yascm 
; loading stdlib.scm
; done loading stdlib.scm
welcome
> (load "examples/mceval.scm")
; loading examples/mceval.scm
; done loading examples/mceval.scm
; ok
> (driver-loop)

;;; M-Eval input:
(define fact (lambda (n) (if (= n 0) 1 (* n (fact (- n 1))))))

;;; M-Eval value:
ok

;;; M-Eval input:
(fact 5)

;;; M-Eval value:
120
```

- [Quine](https://en.wikipedia.org/wiki/Quine_%28computing%29)

```
$ ./yascm 
; loading stdlib.scm
; done loading stdlib.scm
welcome
> ((lambda (x)
      (list x (list (quote quote) x)))
    (quote
      (lambda (x)
        (list x (list (quote quote) x)))))
```

## Testing

```
$ make test
```

## Authors

- Created by hmgle <dustgle@gmail.com>
- [Contributors](https://github.com/hmgle/yascm/graphs/contributors)

## LICENSE

GPL Version 3, see the [COPYING](COPYING) file included in the source distribution.
