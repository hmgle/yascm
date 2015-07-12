(define (map proc items)
  (if (null? items)
    '()
    (cons (proc (car items))
          (map proc (cdr items)))))

(define (apply proc args)
  (eval
    (cons proc
          (map (lambda (x) (list 'quote x))
               args))))

(define (not x)
  (if x #f #t))

(define (< x y) (> y x))

(define (zero? x) (= 0 x))

(define (length items)
  (define (length-iter a count)
    (if (null? a)
        count
        (length-iter (cdr a) (+ 1 count))))
  (length-iter items 0))

(define (for-each f l)
  (cond ((not (null? l))
         (let ((unused (f (car l))))
           (for-each f (cdr l))))))

(define (write x) (display x))

(define true #t)
(define false #f)

(define (equal? a b)
  (cond ((and (pair? a) (pair? b))
         (and (equal? (car a) (car b)) 
             (equal? (cdr a) (cdr b))))
        ((and (not (pair? a)) (not (pair? b)))
         (eq? a b))
        (else false)))

(define (memq item x)
  (cond ((null? x) false)
        ((eq? item (car x)) x)
        (else (memq item (cdr x)))))

(define (append list1 list2)
   (if (null? list1)
       list2
       (cons (car list1) (append (cdr list1) list2))))
