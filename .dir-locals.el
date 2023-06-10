;; configures include directories and the c++ standard library for linters
((c++-mode . ((eval . (set (make-local-variable 'project-path)
                           (expand-file-name (locate-dominating-file default-directory ".dir-locals.el"))))
              (eval . (set (make-local-variable 'project-includes)
                           (mapcar (lambda (p) (concat project-path p))
                                   '("build"
                                     "build/release"
                                     "build/debug"
                                     "include"
                                     "extern/imgui"
                                     "extern/imgui/backends"
                                     "extern/imgui/misc/cpp"
                                     "extern/Catch2/src"
                                     "extern/glfw/include"
                                     "extern/stb"))))
              (eval . (set (make-local-variable 'clang-args)
                           (cons "-std=c++1z"
                                 (mapcar (lambda (p) (concat "-I" p)) project-includes))))
              (eval . (setq-local company-clang-arguments clang-args))
              (eval . (setq-local flycheck-clang-include-path project-includes))
              (eval . (setq-local compile-command "cmake --build build/debug"))
              (flycheck-clang-tidy-extra-options . "-extra-arg=-std=c++1z")
              (flycheck-clang-language-standard . "c++1z"))))
