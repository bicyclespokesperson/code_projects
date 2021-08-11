module Main where

import Data.Maybe
import qualified Data.Text as T
import qualified Data.Text.IO as T


import Control.Monad (foldM)
import System.Directory (doesDirectoryExist, listDirectory) -- from "directory"
--import System.FilePath ((</>), FilePath) -- from "filepath"
import qualified System.FilePath as SF
import Control.Monad.Extra (partitionM) -- from the "extra" package


-- Can't fully handle /* */ case since an include could be commented out due to a previous line
dropComment :: T.Text -> T.Text
dropComment = head . T.splitOn (T.pack "//")

readIncludes :: FilePath -> IO [T.Text]
readIncludes path = do
                  contents <- T.lines <$> T.readFile path
                  return (map (pathFromInclude . dropComment) $ filter (T.isPrefixOf (T.pack "#include")) contents)

pathFromInclude :: T.Text -> T.Text
pathFromInclude = T.drop 1 . T.dropEnd 1 . T.dropAround (`notElem` "<>\"")

-- From: https://stackoverflow.com/questions/51712083/recursively-search-directories-for-all-files-matching-name-criteria-in-haskell
traverseDir :: (FilePath -> Bool) -> (b -> FilePath -> IO b) -> b -> FilePath -> IO b
traverseDir validDir transition = 
  let go state dirPath =
        do names <- listDirectory dirPath
           let paths = map (dirPath </>) names
           (dirPath, filePaths) <- partitionM doesDirectoryExist paths
           state' <- foldM transition state filePaths
           foldM go state' (filter validDir dirPaths)
        in go


main :: IO ()
main = do
         let path = "/home/jeremy/dotfiles/ref/snippets.cpp"
         paths <- readIncludes path
         T.putStrLn $ T.unlines paths
         let results = traverseDir (\_ -> True) (\fs f -> pure (f : fs)) [] "/tmp/somedir"
         putStrLn $ unlines results


