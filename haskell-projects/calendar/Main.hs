module Main where

import           Data.List
import           Data.Time
import           Data.Time.Calendar.WeekDate
import           System.Environment
import           Text.Printf
import           Text.Read

monthNames :: [String]
monthNames =
  [ "January"
  , "February"
  , "March"
  , "April"
  , "May"
  , "June"
  , "July"
  , "August"
  , "September"
  , "October"
  , "November"
  , "December"
  ]

columnsPerDay :: Int
columnsPerDay = 3

columnsPerWeek :: Int
columnsPerWeek = 7 * columnsPerDay

monthsPerRow :: Int
monthsPerRow = 3

getYear :: Day -> Integer
getYear d =
  let (year, _, _) = toGregorian d
   in year

getMonth :: Day -> Int
getMonth d =
  let (_, month, _) = toGregorian d
   in month

getDay :: Day -> Int
getDay d =
  let (_, _, day) = toGregorian d
   in day

-- 1 -> Monday ... 7 -> Sunday
getDayOfWeek :: Day -> Int
getDayOfWeek d =
  let (_, _, day) = toWeekDate d
   in day

daysInYear :: Integer -> [Day]
daysInYear year =
  takeWhile (\x -> getYear x == year) $ iterate succ (fromGregorian year 1 1)

byMonth :: [Day] -> [[Day]]
byMonth = groupBy (\x y -> getMonth x == getMonth y)

byWeek :: [Day] -> [[Day]]
byWeek = groupBy (\_ y -> getDayOfWeek y /= 7)

formatWeek :: [Day] -> String
formatWeek [] = ""
formatWeek xs = padStart xs ++ concatMap (printf "%3d" . getDay) xs ++ padEnd xs
  where
    padStart week =
      replicate (rem (getDayOfWeek (head week)) 7 * columnsPerDay) ' '
    padEnd week =
      let day = getDayOfWeek (last week)
          size = 6 - rem day 7
       in replicate (size * columnsPerDay) ' '

monthHeading :: [Day] -> String
monthHeading month =
  let name = monthNames !! (getMonth (head month) - 1)
      startPadding = quot (columnsPerWeek - length name) 2
      endPadding = columnsPerWeek - startPadding - length name
   in replicate startPadding ' ' ++ name ++ replicate endPadding ' '

formatMonth :: [Day] -> [String]
formatMonth xs = monthHeading xs : map formatWeek (byWeek xs)

{-
   Transpose based on the length of the longest sub list, using a default element to fill in the other lists
-}
transpose' :: a -> [[a]] -> [[a]]
transpose' def rows =
  if any (not . null) rows
    then map (head' def) rows : transpose' def (map tail' rows)
    else []
  where
    head' def' [] = def'
    head' _ x     = head x
    tail' []     = []
    tail' (_:xs) = xs

chunks :: Int -> [a] -> [[a]]
chunks _ [] = []
chunks n xs =
  let (ys, zs) = splitAt n xs
   in ys : chunks n zs

printCalendar :: Integer -> IO ()
printCalendar year =
  let padding = replicate columnsPerWeek ' '
      allMonths = map formatMonth . byMonth $ daysInYear year
      groupedMonths = chunks monthsPerRow allMonths
      printRow =
        putStrLn . unlines . map (intercalate "  ") . transpose' padding
   in mapM_ printRow groupedMonths

usage :: IO ()
usage = putStrLn "Usage: calendar [year]"

main :: IO ()
main = do
  args <- getArgs
  case args of
    [] -> usage
    (x:_) ->
      case readMaybe x of
        Nothing -> usage
        Just a  -> printCalendar a
