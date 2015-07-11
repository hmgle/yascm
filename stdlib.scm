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
